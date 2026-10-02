// Tofu body controller: servos, gait, IMU, lidar forwarding, safety.

#include <Arduino.h>

#include "tofu_protocol.h"

struct CmdVel {
  float vx = 0;   // m/s forward
  float vy = 0;   // m/s left
  float yaw = 0;  // rad/s counter-clockwise
};

static CmdVel g_cmd;
static unsigned long g_last_cmd_ms = 0;

// Called by the network layer whenever a velocity command arrives.
void onCmdVel(const CmdVel& cmd) {
  g_cmd = cmd;
  g_last_cmd_ms = millis();
}

// Safety runs locally so a Wi-Fi drop can never leave Tofu walking blind.
static bool commandTimedOut() {
  return millis() - g_last_cmd_ms > tofu::kCmdVelTimeoutMs;
}

void setup() {
  Serial.begin(115200);
  Serial.println("tofu body: boot");
  // TODO: init servo bus, IMU, cliff sensors, battery gauge, lidar UART, Wi-Fi.
}

void loop() {
  CmdVel active = commandTimedOut() ? CmdVel{} : g_cmd;
  // TODO: if a cliff sensor detects a drop, zero forward motion.
  // TODO: feed `active` into the gait generator and write joint targets.
  (void)active;
  delay(10);
}
