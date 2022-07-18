#include "GUI.h"
#include "GlobalVars.h"
#include "Ball.h"
#include "Balls.h"
#include "GenericData.h"
#include "TableInfo.h"
#include "Memory.h"
#include "Offsets.h"
#include <cmath>
#include <string>
#include <intrin.h>
#include "const.h"
#include <vector>
#include "Prediction.h"
#include "Colors.hpp"
#include "GameManager.h"
#include "MenuManager.h"
#include "AdsManager.h"
#include "CodeManager.h"
#include "globalFunks.h"
#include <array>

#include "D2DOverlay/DirectOverlay.h"
#include "skCrypt.h"
#include "Utils.h"

#if defined(_WIN32)
#pragma comment(lib, "D2DOverlay/lib/x86/D2DOverlay.lib")
#elif
#pragma comment(lib, "D2DOverlay/lib/x64/D2DOverlay.lib")
#endif

namespace AEngineRenderWindow
{
    HWND  AEngineRenderWindow::window;
}

UINT_PTR      slowestTimerID, slowTimerID, fastTimerID, heartbeatTimerID;
void CALLBACK timerCallback(HWND hwnd, UINT uMsg, UINT_PTR timerId, DWORD dwTime);

void drawLine(const Vector2D& src, const Vector2D& dst, const Color* color, float transparency, float thickness = 1.f)
{
    float point[4];
    float alpha = 1.f - transparency * 0.01f;

    point[0] = static_cast<float>(src.x);
    point[1] = static_cast<float>(src.y);
    point[2] = static_cast<float>(dst.x);
    point[3] = static_cast<float>(dst.y);
    DrawLine(point[0], point[1], point[2], point[3], thickness, color->r, color->g, color->b, alpha);
}

void drawCircle(const Vector2D& point, vec_t radius, const Color* color, float transparency, float thickness = 1.f, bool filled = true)
{
    float pointf[2], radiusf;
    float alpha = 1.f - transparency * 0.01f;
    
    pointf[0] = static_cast<float>(point.x);
    pointf[1] = static_cast<float>(point.y);
    radiusf   = static_cast<float>(radius);
    DrawCircle(pointf[0], pointf[1], radiusf, thickness, color->r, color->g, color->b, alpha, filled);
}

bool isValidPredictionState()
{
    int gameState = GameManager::getState(gGlobalVars);

    switch (gGlobalVars->features.esp.state)
    {
    case 0:
        return (gameState == 4);
    case 1:
        return (gameState == 7);
    default:
        return (gameState == 4 || gameState == 7);
    }
}

int getLinesStartIndex(int linesCount)
{
    float scale = static_cast<float>(gGlobalVars->features.esp.displayPercantage) / 100.f;

    return (linesCount - static_cast<int>(roundf(scale * static_cast<float>(linesCount - 1))));
}

void drawAimLine()
{
    UINT8                          activePockets[6];
    bool                           draw, predictionState;
    float                          tickness;
    Color*                         color;
    SIZE_T                         i, j, countA, countB;
    Vector2D                       srcPoint, dstPoint;
    Prediction::PredictionInfo*    predictionInfo;
    const std::array<Vector2D, 6>* pocketsPositions;
    const auto&                    espFeatures = gGlobalVars->features.esp;

    if (MenuManager::isInGame(gGlobalVars) && isValidPredictionState() && gPrediction->init()) {
        if (espFeatures.predicionPath) {
            countA = gPrediction->guiData.ballsCount;
            for (i = 0UL; i < countA; i++) {
                predictionInfo = &gPrediction->guiData.info[i];
                draw = (predictionInfo->initialPos != predictionInfo->predictedPosition);
                if (draw) {
                    color = &colors[i % colors.size()];
                    tickness = espFeatures.ballLineThickness + ((i > 8) ? 1.f : 4.f);
                    countB = predictionInfo->positions.size();
                    for (j = getLinesStartIndex(countB); j < countB; j++) {
                        srcPoint = predictionInfo->positions[j - 1].toScreen();
                        dstPoint = predictionInfo->positions[j].toScreen();
                        draw = (srcPoint != dstPoint);
                        if (draw)
                            drawLine(srcPoint, dstPoint, color, espFeatures.ballTransparency, tickness);
                    }

                    drawCircle(dstPoint, espFeatures.ballCircleRadius * gGlobalVars->gameloopWindowInfo.scale.x, color, espFeatures.ballTransparency, 1.f, espFeatures.ballCircleFilled);
                }
            }
        }
        if (espFeatures.shotState) {
            /*ZeroMemory(activePockets, sizeof(activePockets));
            countA = gPrediction->guiData.ballsCount;
            for (i = 0UL; i < countA; i++) {
                predictionInfo = &gPrediction->guiData.info[i];
                if (predictionInfo->originalOnTable != predictionInfo->onTable) {
                    activePockets[predictionInfo->pocketIndex]++;
                }*/
            ZeroMemory(activePockets, sizeof(activePockets));
            countA = gPrediction->guiData.ballsCount;
            for (i = 0UL; i < countA; i++) {
                predictionInfo = &gPrediction->guiData.info[i];
                if (predictionInfo->originalOnTable != predictionInfo->onTable) {
                    activePockets[predictionInfo->pocketIndex]++;
                }
            }

            pocketsPositions = &TableInfo::getPocketsPositions();
            predictionState  = gPrediction->guiData.state;
            countA           = gPrediction->guiData.ballsCount;
            for (i = 0UL; i < pocketsPositions->size(); i++) {
                if (predictionState && activePockets[i] > 0)
                    color = &colors[6]; // green
                else
                    color = &colors[3]; // red

                srcPoint = pocketsPositions->at(i).toScreen();
                drawCircle
                (
                    srcPoint,
                    espFeatures.shotStateCircleRadius * gGlobalVars->gameloopWindowInfo.scale.x,
                    color, 
                    espFeatures.shotStateTransparency,
                    espFeatures.shotStateCircleThickness,
                    espFeatures.shotStateCircleFilled
                );
            }
        }
    }
}

void drawLoop(int width, int height) {
    if (gGlobalVars->features.esp.predicionPath || gGlobalVars->features.esp.shotState)
        drawAimLine();
}

void GUI::init()
{
    HWND gameLoopWindow;

    gameLoopWindow = FindWindow(skCrypt("TXGuiFoundation"), skCrypt("Gameloop"));
    if (gameLoopWindow)
        AEngineRenderWindow::window = FindWindowEx(gameLoopWindow, 0, skCrypt("AEngineRenderWindowClass"), skCrypt("AEngineRenderWindow"));
    if (AEngineRenderWindow::window) {
        DirectOverlaySetOption(D2DOV_FONT_IMPACT | D2DOV_VSYNC);
        DirectOverlaySetup(drawLoop, AEngineRenderWindow::window);
        heartbeatTimerID = SetTimer(nullptr, 0UL, 1000U * 60U * 1U, timerCallback);
        slowestTimerID   = SetTimer(nullptr, 0UL, 1000U * 60U * 5U, timerCallback);
        slowTimerID      = SetTimer(nullptr, 0UL, 5000U, timerCallback);
        fastTimerID      = SetTimer(nullptr, 1UL, 1000U, timerCallback);
    }
}

void GUI::end()
{
    KillTimer(nullptr, heartbeatTimerID);
    KillTimer(nullptr, slowestTimerID);
    KillTimer(nullptr, slowTimerID);
    KillTimer(nullptr, fastTimerID);
}

static void CALLBACK timerCallback(HWND hwnd, UINT uMsg, UINT_PTR timerId, DWORD dwTime)
{
    RECT            buffer;
    Vector2D        size;
    Vector4D        tableBuffer;
    std::string     heartbeatResponse;
    constexpr vec_t widthScale = 6.05678233438, heightScale = 3.62416107383, bottomScale = 6.79245283019;

    if (timerId == slowTimerID) {
        if (IsWindow(AEngineRenderWindow::window)) {
            GetWindowRect(AEngineRenderWindow::window, &buffer);
            tableBuffer.x = static_cast<vec_t>(buffer.left);
            tableBuffer.y = static_cast<vec_t>(buffer.top);
            tableBuffer.z = static_cast<vec_t>(buffer.right);
            tableBuffer.w = static_cast<vec_t>(buffer.bottom);

            size.x = tableBuffer.z - tableBuffer.x;
            size.y = tableBuffer.w - tableBuffer.y;

            gGlobalVars->gameloopWindowInfo.scale.x = size.x / static_cast<vec_t>(GetSystemMetrics(SM_CXSCREEN));
            gGlobalVars->gameloopWindowInfo.scale.y = size.y / static_cast<vec_t>(GetSystemMetrics(SM_CYSCREEN));

            tableBuffer.x = size.x / widthScale;
            tableBuffer.y = size.y / heightScale;
            tableBuffer.z = size.x - size.x / widthScale;
            tableBuffer.w = size.y - size.y / bottomScale;
            TableInfo::Position::set(tableBuffer.x, tableBuffer.y, tableBuffer.z, tableBuffer.w);

            tableBuffer.z -= tableBuffer.x;
            tableBuffer.w -= tableBuffer.y;
            TableInfo::setSize(tableBuffer.z, tableBuffer.w);
        }

        if (gGlobalVars->features.adBlock)
            AdsManager::disableAdBreakScreen(gGlobalVars);
    }
    else if (timerId == fastTimerID) {
        if (gGlobalVars->features.esp.wideGuideLine) {
            gGlobalVars->memory->write(gGlobalVars->gameModuleBase + gOffsets::CUE_PROPERTIES_ACCURACY, 10000.0);
        }
    }
    else if (timerId == slowestTimerID) {
        gCodeManager->clear();
    }

}
