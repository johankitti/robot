#pragma once

// Timing constants shared by the body firmware, head firmware and server.

namespace tofu {

// If no velocity command arrives within this window, the body stops walking.
constexpr unsigned long kCmdVelTimeoutMs = 300;

}  // namespace tofu
