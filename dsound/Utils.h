#pragma once

#include <basetsd.h>

void launch8BPGame();
void notifyUser(const char* message, const char* title = "");
void fatal(const char* error, const char* _where, UINT32 errorCode);

int   getRandomInt(const int& min, const int& max);
float getRandomFloat(const float& min, const float& max);