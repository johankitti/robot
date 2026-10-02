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
Position estimates from leg motion (leg odometry) are poor because feet slip. The LD06 lidar gives slam_toolbox scans to correct against. Leg odometry and IMU data are fused only as a short-term prior.

### Safety runs on the robot
- **Command timeout:** if no `cmd_vel` arrives for 300 ms, the body firmware stops walking and settles into a stand.
- **Cliff sensors:** downward-facing ToF sensors at the front and rear. A detected drop stops forward motion immediately, without waiting for the server.
- **Low-battery cutoff:** below a hard voltage floor, Tofu lies down and powers down the servos, whatever the server says.

### LLM actions through tool calls
The brain gives Claude tools such as `walk`, `set_face`, `go_to(room)`, `go_charge`, and `get_status`, instead of parsing free-form JSON from its reply. Results from the tools (for example "battery 15%") go back to the model so it can respond naturally.

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
