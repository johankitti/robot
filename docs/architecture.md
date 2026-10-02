# Tofu architecture

## System diagram

```
┌──────────────── ROBOT ────────────────┐          ┌──────────── HOME SERVER ────────────┐
│ ESP32-S3 "body"                       │          │ N100 mini PC or Pi 5 (8GB)          │
│  • gait + inverse kinematics (local)  │  Wi-Fi   │                                     │
│  • 12× STS3215 bus servos (one UART)  │◄────────►│ ROS 2: slam_toolbox + Nav2          │
│  • IMU, battery gauge                 │ micro-ROS│   ← lidar scans, IMU, leg odometry  │
│  • LD06 lidar → forwards raw scans    │  / UDP   │   → cmd_vel (vx, vy, yaw)           │
│  • cliff sensors (local safety stop)  │          │                                     │
│                                       │          │ Voice: wake word → STT → Claude     │
│ ESP32-S3 "head"                       │          │   (tool calls) → TTS → audio back   │
│  • round screen face                  │          │                                     │
│  • I2S mic + speaker amp              │          │ Behavior: battery low → go to dock  │
│  • camera (for dock AprilTag)         │          │   → AprilTag alignment → charge     │
└───────────────────────────────────────┘          └─────────────────────────────────────┘
```

## Key design decisions

### The brain lives on the server
The ESP32s handle only real-time, local work: gait timing, servo control, reading sensors, audio input and output, and drawing the face. Everything computationally heavy runs on the server: SLAM, path planning, speech-to-text, the LLM, and text-to-speech. This keeps the robot light and its battery life long, and lets the software change without reflashing.

### Two microcontrollers
One ESP32-S3 isn't enough for the servo bus, lidar, IMU, camera, I2S mic, I2S amp and display all at once, either in pins or in processing. Splitting them also keeps audio and camera traffic from disrupting gait timing.

### The legs look like wheels to Nav2
The body firmware accepts a `cmd_vel` (vx, vy, yaw rate), the same velocity command a wheeled robot takes, and turns it into a gait. Nav2 doesn't need to know Tofu has legs.

### Lidar localization, not step counting
Position estimates from leg motion (leg odometry) are poor because feet slip. The LD06 lidar gives slam_toolbox scans to correct against. Leg odometry and IMU data are fused only as a short-term prior. See [Navigation](#navigation) for the full pipeline.

### Safety runs on the robot
- **Command timeout:** if no `cmd_vel` arrives for 300 ms, the body firmware stops walking and settles into a stand.
- **Cliff sensors:** downward-facing ToF sensors at the front and rear. A detected drop stops forward motion immediately, without waiting for the server.
- **Low-battery cutoff:** below a hard voltage floor, Tofu lies down and powers down the servos, whatever the server says.

### LLM actions through tool calls
The brain gives Claude tools such as `walk`, `set_face`, `go_to(room)`, `go_charge`, and `get_status`, instead of parsing free-form JSON from its reply. Results from the tools (for example "battery 15%") go back to the model so it can respond naturally.

### Navigation

Tofu navigates the way a lidar robot vacuum does. The server keeps a 2D floor plan of the apartment, works out where Tofu is on it from the lidar, plans a path, and sends velocity commands to the body.

#### Phase 1: Map the apartment once
1. Drive Tofu through every room by remote control (gamepad or phone).
2. The LD06 lidar scans 360° at about 10 Hz. slam_toolbox stitches the scans into a floor plan with 5 cm cells, each marked free, occupied or unknown.
3. SLAM (simultaneous localization and mapping) builds the map and tracks Tofu's position at the same time. When Tofu re-enters an area it has already mapped, it recognizes it and corrects built-up drift (loop closure).
4. Save the map, then mark named spots on it (`kitchen`, `living_room`, `dock`, ...). These are the targets for the brain's `go_to(room)` tool.
5. Draw no-go zones (keepout areas) over glass doors, mirrors, cable nests and the top of any stairs.

#### Phase 2: Navigate every day
```
lidar scans ──► localization ──► "I'm at (x, y), facing θ" on the map
IMU + legs ──► EKF fusion ─────┘   (smooths position between scans)

goal "kitchen" ──► global planner ──► route through the map
                   local controller ──► cmd_vel (vx, vy, yaw) ──► Wi-Fi ──► body ESP32 ──► gait
```
1. **Localization:** each scan is matched against the saved map (slam_toolbox localization mode, or AMCL).
2. **Short-term motion estimate:** between scans, IMU data and leg motion are combined in an EKF (robot_localization). This estimate drifts, but the lidar corrects it several times a second.
3. **Planning:** Nav2's global planner finds a route around mapped walls and furniture. The local controller (MPPI, configured for a robot that can move sideways) steers along the route and avoids new obstacles: moved chairs, bags, feet.
4. **Walking:** the controller outputs `cmd_vel`, and the body firmware turns it into a gait.
5. **Getting stuck:** if Tofu is blocked, Nav2 recovery behaviors back it up, turn it and replan. After repeated failures, the brain tells the user where Tofu is stuck.

#### Issues specific to walking
- **Body bounce:** each step tilts the lidar, so a scan can hit the floor or a table edge and look like a wall. Mount the lidar on top of the body, and use the IMU to drop or correct scans taken at large tilt. A smooth gait directly improves map quality.
- **Moving sideways:** four legs can walk sideways, so Nav2 is configured for a robot that can (`vy` enabled). Tofu can sidestep obstacles and line up precisely with the dock.
- **Wi-Fi delay:** at about 0.2 m/s, 50 ms of delay equals about 1 cm of travel, which is negligible. If the connection drops entirely, the body's command timeout and cliff sensors take over.

#### What a 2D lidar can't see
The lidar scans one flat slice at about 15–20 cm height.

| Problem | Example | Mitigation |
|---|---|---|
| Things lower than the scan | Cables, socks, toys, pet bowls | Forward-facing ToF sensors; later a depth camera |
| Things higher than the scan with a gap below | Tabletops, chair seats, sofas on legs | Usually fine, since Tofu is short; only the legs may be seen |
| Glass and mirrors | Balcony door, mirrored wardrobe | No-go zones on the map |
| Very dark surfaces | Black furniture | Usually fine with the LD06; check the map for gaps |
| Stairs and drop-offs | Invisible to a flat scan | Cliff ToF sensors with a local stop on the robot, plus a no-go zone |

If low clutter becomes a real problem: an ESP32 can't stream depth images, so add a small onboard computer (e.g. a Pi Zero 2 W) to forward a depth camera to the server.

### Docking
1. Nav2 drives to a staging pose about 50 cm in front of the dock.
2. The head camera detects the dock's AprilTag, and a closed-loop controller lines Tofu up for the final approach.
3. Tofu walks forward onto the dock and lowers its belly onto pogo-pin or magnetic contacts.
4. The battery gauge confirms charging current. If it doesn't, Tofu backs off and retries.

## Open questions
- Transport: micro-ROS over Wi-Fi vs a custom UDP protocol with a bridge node on the server
- STT: local faster-whisper vs a cloud API
- TTS voice and personality
- Robot size and weight budget, to be finalized after the SpotMicro remix is chosen
