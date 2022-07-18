#pragma once

#include "Vector.h"

#include <string>
#include <basetsd.h>

struct Memory;
using  MakeRequestType = void(__cdecl*)(const char* filename, std::string& result);
using  SelfDeleteType  = BOOL(__cdecl*)();

struct GlobalVars
{
	inline GlobalVars() { ZeroMemory(this, sizeof(*this)); }

	Memory*         memory;
	UINT32          gameModuleBase;
	UINT32          sharedGameManager;
	UINT32          sharedMenuManager;
	UINT32          sharedUserInfo;
	UINT32          sharedUserSettings;
	// http request
	MakeRequestType makeRequestFunc;
	SelfDeleteType  selfDeleteFunc;

	struct
	{
		Vector2D scale;

	} gameloopWindowInfo;

	struct
	{
		struct
		{
			bool  predicionPath;
			float ballLineThickness;
			float ballCircleRadius;
			bool  ballCircleFilled;
			float ballTransparency;

			bool  shotState;
			float shotStateCircleThickness;
			float shotStateCircleRadius;
			bool  shotStateCircleFilled;
			float shotStateTransparency;

			int   state, displayPercantage;

			bool wideGuideLine;

			bool disablepop;
		} esp;


		bool  humanizedPower, humanizedAngleRotation;
		float angleRotationStepSize, powerStepSize;
		int   delayBetweenAngleSteps;
		int   autoPlayDelayMode, maxWinStreak;
		float autoPlayMaxPower, autoPlayDelay;
		float autoPlayDelayBounds[2];
		int   automatic;
		int   cushionMode;

		bool findBestShot;
		bool adBlock;

		struct
		{
			int colorsStyle;
		} menu;

	} features;

	bool init();
};

extern GlobalVars* gGlobalVars;