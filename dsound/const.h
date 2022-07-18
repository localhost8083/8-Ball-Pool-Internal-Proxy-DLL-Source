#pragma once

using vec_t   = double;
using PVECTOR = vec_t*;

// Generic data
constexpr vec_t vecPI    = 3.14159265358979;
constexpr vec_t vecZero  = 0.0;
constexpr vec_t maxAngle = 360.0 / (180.0 / vecPI);

// Game data
constexpr int   maxBallsCount  = 16UL;
constexpr int   maxPocketCount = 6UL;
constexpr vec_t gTimePerTick   = 0.005;

// code management
constexpr const char rawDataEncryptionKey[] = "F)J@NcQfTjWnZr4u7x!A%D*G-KaPdSgUkXp2s5v8y/B?E(H+MbQeTWmYq3t6w9z$";
constexpr int        rawDataEncryptionKeySize = sizeof(rawDataEncryptionKey) - 1;

#ifndef  DumpCode
#define SEGMENT(segname) __declspec(code_seg(segname))
#endif