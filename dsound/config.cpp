#include "config.h"
#include <nlohmann/json.hpp>

#include <fstream>
#include "GlobalVars.h"
#include "skCrypt.h"
#include "UserSettings.h"

#define CONFIG_NAME skCrypt("config.json")

void Config::save()
{
	std::ofstream  configFile;
	nlohmann::json j;

	j.emplace(skCrypt("colorsStyle"), gGlobalVars->features.menu.colorsStyle);

	j.emplace(skCrypt("adBlock"), gGlobalVars->features.adBlock);
	j.emplace(skCrypt("automatic"), gGlobalVars->features.automatic);
	j.emplace(skCrypt("humanizedPower"), gGlobalVars->features.humanizedPower);
	j.emplace(skCrypt("humanizedAngleRotation"), gGlobalVars->features.humanizedAngleRotation);
	j.emplace(skCrypt("autoPlayDelay"), gGlobalVars->features.autoPlayDelay);
	j.emplace(skCrypt("autoPlayDelayBounds[0]"), gGlobalVars->features.autoPlayDelayBounds[0]);
	j.emplace(skCrypt("autoPlayDelayBounds[1]"), gGlobalVars->features.autoPlayDelayBounds[1]);
	j.emplace(skCrypt("autoPlayDelayMode"), gGlobalVars->features.autoPlayDelayMode);
	j.emplace(skCrypt("autoPlayMaxPower"), gGlobalVars->features.autoPlayMaxPower);
	j.emplace(skCrypt("maxWinStreak"), gGlobalVars->features.maxWinStreak);

	j.emplace(skCrypt("angleRotationStepSize"), gGlobalVars->features.angleRotationStepSize);
	j.emplace(skCrypt("delayBetweenAngleSteps"), gGlobalVars->features.delayBetweenAngleSteps);
	j.emplace(skCrypt("powerStepSize"), gGlobalVars->features.powerStepSize);

	j.emplace(skCrypt("predicionPath"), gGlobalVars->features.esp.predicionPath);
	j.emplace(skCrypt("ballLineThickness"), gGlobalVars->features.esp.ballLineThickness);
	j.emplace(skCrypt("ballCircleFilled"), gGlobalVars->features.esp.ballCircleFilled);
	j.emplace(skCrypt("ballCircleRadius"), gGlobalVars->features.esp.ballCircleRadius);

	j.emplace(skCrypt("shotState"), gGlobalVars->features.esp.shotState);
	j.emplace(skCrypt("shotStateCircleFilled"), gGlobalVars->features.esp.shotStateCircleFilled);
	j.emplace(skCrypt("shotStateCircleRadius"), gGlobalVars->features.esp.shotStateCircleRadius);
	j.emplace(skCrypt("shotStateCircleThickness"), gGlobalVars->features.esp.shotStateCircleThickness);
	j.emplace(skCrypt("ballTransparency"), gGlobalVars->features.esp.ballTransparency);

	j.emplace(skCrypt("displayPercantage"), gGlobalVars->features.esp.displayPercantage);
	j.emplace(skCrypt("shotState"), gGlobalVars->features.esp.shotState);
	j.emplace(skCrypt("state"), gGlobalVars->features.esp.state);
	j.emplace(skCrypt("shotStateTransparency"), gGlobalVars->features.esp.shotStateTransparency);

	j.emplace(skCrypt("wideGuideLine"), gGlobalVars->features.esp.wideGuideLine);

	configFile = std::ofstream(CONFIG_NAME);
	configFile << std::setw(4) << j << std::endl;
	configFile.close();
}

template <typename T>
inline void loadVarible(const char* varibleName, T& data, const nlohmann::json& j)
{
	const auto& varibleData = j.find(varibleName);
	if (varibleData == j.end())
		return;

	data = (*varibleData).get<T>();
}


void Config::load()
{
	std::ifstream inputFile = std::ifstream(CONFIG_NAME);

	if (!inputFile.good())
		return;

	nlohmann::json j;

	try
	{
		inputFile >> j;
	}
	catch (...)
	{
		goto endLabel;
	}

	if (j.empty())
		goto endLabel;

	loadVarible(skCrypt("colorsStyle"), gGlobalVars->features.menu.colorsStyle, j);

	loadVarible(skCrypt("adBlock"), gGlobalVars->features.adBlock, j);
	loadVarible(skCrypt("automatic"), gGlobalVars->features.automatic, j);
	loadVarible(skCrypt("humanizedPower"), gGlobalVars->features.humanizedPower, j);
	loadVarible(skCrypt("humanizedAngleRotation"), gGlobalVars->features.humanizedAngleRotation, j);
	loadVarible(skCrypt("autoPlayDelay"), gGlobalVars->features.autoPlayDelay, j);
	loadVarible(skCrypt("autoPlayDelayBounds[0]"), gGlobalVars->features.autoPlayDelayBounds[0], j);
	loadVarible(skCrypt("autoPlayDelayBounds[1]"), gGlobalVars->features.autoPlayDelayBounds[1], j);
	loadVarible(skCrypt("autoPlayDelayMode"), gGlobalVars->features.autoPlayDelayMode, j);
	loadVarible(skCrypt("autoPlayMaxPower"), gGlobalVars->features.autoPlayMaxPower, j);
	loadVarible(skCrypt("maxWinStreak"), gGlobalVars->features.maxWinStreak, j);

	loadVarible(skCrypt("angleRotationStepSize"), gGlobalVars->features.angleRotationStepSize, j);
	loadVarible(skCrypt("delayBetweenAngleSteps"), gGlobalVars->features.delayBetweenAngleSteps, j);
	loadVarible(skCrypt("powerStepSize"), gGlobalVars->features.powerStepSize, j);

	loadVarible(skCrypt("predicionPath"), gGlobalVars->features.esp.predicionPath, j);
	loadVarible(skCrypt("ballLineThickness"), gGlobalVars->features.esp.ballLineThickness, j);
	loadVarible(skCrypt("ballCircleFilled"), gGlobalVars->features.esp.ballCircleFilled, j);
	loadVarible(skCrypt("ballCircleRadius"), gGlobalVars->features.esp.ballCircleRadius, j);

	loadVarible(skCrypt("shotState"), gGlobalVars->features.esp.shotState, j);
	loadVarible(skCrypt("shotStateCircleFilled"), gGlobalVars->features.esp.shotStateCircleFilled, j);
	loadVarible(skCrypt("shotStateCircleRadius"), gGlobalVars->features.esp.shotStateCircleRadius, j);
	loadVarible(skCrypt("shotStateCircleThickness"), gGlobalVars->features.esp.shotStateCircleThickness, j);
	loadVarible(skCrypt("ballTransparency"), gGlobalVars->features.esp.ballTransparency, j);

	loadVarible(skCrypt("displayPercantage"), gGlobalVars->features.esp.displayPercantage, j);
	loadVarible(skCrypt("shotState"), gGlobalVars->features.esp.shotState, j);
	loadVarible(skCrypt("state"), gGlobalVars->features.esp.state, j);
	loadVarible(skCrypt("shotStateTransparency"), gGlobalVars->features.esp.shotStateTransparency, j);

	loadVarible(skCrypt("wideGuideLine"), gGlobalVars->features.esp.wideGuideLine, j);

	UserSettings::setWideGuideLine(gGlobalVars, gGlobalVars->features.esp.wideGuideLine);

endLabel:
	inputFile.close();
}
