# server/nav

ROS 2 workspace for mapping, navigation and docking. To be created as packages:

- `tofu_bringup`: launch files and parameters
- `tofu_bridge`: connects the body ESP32 (lidar scans, IMU, odometry in; `cmd_vel` out)
- `tofu_docking`: AprilTag alignment and the charge-confirmation behavior

Stack: slam_toolbox (mapping), Nav2 (navigation), apriltag_ros (dock detection).
