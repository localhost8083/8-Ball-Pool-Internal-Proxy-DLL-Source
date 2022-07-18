#pragma once

#include <Windows.h>

namespace AEngineRenderWindow
{
	extern HWND  window;
}

struct GUI
{
	static void init();
	static void end();
};

