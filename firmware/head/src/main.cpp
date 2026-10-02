// Tofu head controller: face display, mic and speaker streaming, camera.

#include <Arduino.h>

#include "tofu_protocol.h"

void setup() {
  Serial.begin(115200);
  Serial.println("tofu head: boot");
  // TODO: init display, I2S mic, I2S amp, camera, Wi-Fi.
}

void loop() {
  // TODO: render face, stream mic audio to server, play TTS audio from server.
  delay(10);
}
