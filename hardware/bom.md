# Bill of materials (draft)

Prices are rough USD estimates. Nothing here is final until the frame design is chosen.

## Body

| Qty | Part | Purpose | ~Price |
|---|---|---|---|
| 1 | ESP32-S3 dev board (N16R8) | Body controller | $10 |
| 12 | Feetech STS3215 serial bus servo | Leg joints, 3 per leg | $180 |
| 1 | Feetech URT-1 or a half-duplex UART adapter circuit | Servo bus interface | $10 |
| 1 | LDRobot LD06 (or LD19) 2D lidar | Mapping and localization | $70 |
| 1 | BNO085 IMU | Orientation, gait stabilization | $20 |
| 2–4 | VL53L0X / VL53L1X ToF sensor | Cliff detection | $15 |
| 1 | INA226 current/voltage monitor | Battery gauge | $5 |
| 1 | 2S or 3S Li-ion pack + BMS | Power | $40 |
| 1 | High-current buck converter (servo rail) | Servo power | $15 |
| 1 | 5V buck converter (logic rail) | ESP32s, sensors | $5 |

## Head

| Qty | Part | Purpose | ~Price |
|---|---|---|---|
| 1 | ESP32-S3 with camera connector (e.g. XIAO ESP32S3 Sense) | Head controller, camera | $15 |
| 1 | Round display (GC9A01 1.28" or larger) | Face | $10 |
| 1 | INMP441 I2S MEMS microphone | Voice input | $5 |
| 1 | MAX98357A I2S amp + small speaker | Voice output | $10 |

## Dock

| Qty | Part | Purpose | ~Price |
|---|---|---|---|
| 1 | Pogo pins or magnetic contacts (pair) | Charging contacts | $10 |
| 1 | Li-ion charger module matched to the pack | Charging | $15 |
| 1 | Printed AprilTag | Final alignment | $0 |

## Server

| Qty | Part | Purpose | ~Price |
|---|---|---|---|
| 1 | Intel N100 mini PC (or Pi 5 8GB) | SLAM, Nav2, voice pipeline | $150 |

**Estimated total:** about $600–700, plus filament and fasteners.
