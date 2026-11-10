// META NO_MERGE
// =====================================================================
// zJumpDistance: configurable jump distance & height for gothic
// =====================================================================
// you can configure jump parameters either directly in this script
// or in system\Gothic.ini under the [ZJUMPDISTANCE] section.
//
// multipliers:
// 1.0 = vanilla distance
// 1.5 = +50% distance
// 1.6 = +60% distance (matches +60% running speed in zSpeedyAnis)
// 2.0 = double distance
// =====================================================================

// master multiplier applied to all jumps (horizontal distance):
const float z_JumpMultiplier = 1.0;

// multiplier for normal running and standing forward jump:
const float z_JumpForwardMultiplier = 1.0;

// multiplier for acrobatics running forward jump:
const float z_JumpAcrobaticsMultiplier = 1.0;

// multiplier for standing vertical jump up:
const float z_JumpVerticalMultiplier = 1.0;

// multiplier for jump height (vertical impulse):
const float z_JumpHeightMultiplier = 1.0;

// built-in fps fix for jump distance (4 cases with Union_JumpAcrobaticsNoFps):
//   0 + Union_JumpAcrobaticsNoFps absent  -> vanilla fps-dependent jump base + multipliers
//   1 + Union_JumpAcrobaticsNoFps absent  -> zJumpDistance applies fps fix + multipliers
//   0 + Union_JumpAcrobaticsNoFps present -> Union_JumpAcrobaticsNoFps applies fps fix + zJumpDistance applies multipliers
//   1 + Union_JumpAcrobaticsNoFps present -> Union_JumpAcrobaticsNoFps applies fps fix (no double fix) + zJumpDistance applies multipliers
const int z_FixFpsJump = 1;
