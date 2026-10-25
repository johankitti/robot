# server/nav

ROS 2 workspace for mapping, navigation and docking. To be created as packages:

- `tofu_bringup`: launch files and parameters
- `tofu_bridge`: connects the body ESP32 (lidar scans, IMU, odometry in; `cmd_vel` out)
- `tofu_docking`: AprilTag alignment and the charge-confirmation behavior

Stack: slam_toolbox (mapping), Nav2 (navigation), apriltag_ros (dock detection).

## Links

| Project | Link | Used for |
|---|---|---|
| ROS 2 Jazzy | https://docs.ros.org/en/jazzy/ | Framework (LTS, Ubuntu 24.04) |
| ldlidar_stl_ros2 | https://github.com/ldrobotSensorTeam/ldlidar_stl_ros2 | LD06/LD19 driver, for testing the lidar over USB |
| slam_toolbox | https://github.com/SteveMacenski/slam_toolbox | Mapping, then localization against the saved map |
| Nav2 | https://github.com/ros-navigation/navigation2 ([docs](https://docs.nav2.org)) | Planning, MPPI controller, recovery, keepout zones, AMCL, docking server |
| robot_localization | https://github.com/cra-ros-pkg/robot_localization | EKF fusing IMU and leg odometry |
| RViz 2 | https://github.com/ros2/rviz | Viewing the map, scans and pose |
| micro-ROS for PlatformIO | https://github.com/micro-ROS/micro_ros_platformio | ROS 2 on the body ESP32 (`tofu_bridge` link) |
| apriltag_ros | https://github.com/christianrauch/apriltag_ros | Dock tag detection |

Check Nav2's built-in docking server (originally `opennav_docking`) before writing much of `tofu_docking`; it may cover most of it.

Alternatives: [Cartographer ROS](https://github.com/cartographer-project/cartographer_ros) (SLAM, mostly in maintenance mode), [Foxglove](https://foxglove.dev) (browser-based RViz alternative).
