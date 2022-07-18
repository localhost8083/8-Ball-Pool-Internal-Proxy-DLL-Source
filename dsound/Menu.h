#pragma once

#include <Windows.h>

struct Menu
{
	static bool init(HINSTANCE instance); 
	bool runKeyCheck();
	static void runLoop(), end(HINSTANCE instance);
};

