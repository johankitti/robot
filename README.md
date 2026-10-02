# Tofu

A four-legged, 3D-printed home robot with a screen for a face. Tofu walks around the house on its own, talks with you, and goes back to its dock to charge when its battery gets low.

## How it's split up

Tofu itself only carries the parts that have to be on the robot: motors, sensors, face, mic and speaker. The heavy work (mapping, navigation, speech, the LLM) runs on a home server over Wi-Fi.

| Folder | Runs on | What it does |
|---|---|---|
| `firmware/body/` | ESP32-S3 in the torso | Servo bus, gait and inverse kinematics, IMU, battery gauge, lidar forwarding, safety stop |
| `firmware/head/` | ESP32-S3 in the head | Screen face, mic and speaker audio streaming, camera |
| `firmware/shared/` | both | Protocol definitions shared by body and head |
| `server/nav/` | home server | ROS 2: SLAM, Nav2, docking behavior |
| `server/brain/` | home server | Voice pipeline: wake word, STT, Claude with tool calls, TTS |
| `sim/` | dev machine | MuJoCo model for gait and navigation testing |
| `hardware/` | none (design files) | CAD, bill of materials, wiring |
| `docs/` | none | Architecture and design notes |

See [docs/architecture.md](docs/architecture.md) for the full design.

## Build roadmap

0. **Warm-up:** build a stock [Sesame](https://github.com/dorianborian/sesame-robot/) to learn servo calibration, gait tuning and face animation.
1. **Body:** print a SpotMicro-based frame with STS3215 bus servos, and get it standing and walking under remote control.
2. **Face and voice:** head ESP32, screen, and the audio pipeline to the server.
3. **Mapping:** lidar scans streamed to the server, then map the house with slam_toolbox.
4. **Navigation:** Nav2 point-to-point ("Tofu, go to the kitchen").
5. **Docking:** charging dock with an AprilTag, alignment behavior, and the battery-low trigger.

Stairs are out of scope. Tofu stays on one floor, and cliff sensors keep it from falling down stairs.
