#include "Menu.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"

#include <d3d9.h>
#include <tchar.h>

#include "config.h"
#include "resource.h"
#include "GlobalVars.h"
#include "UserSettings.h"

#include <array>
#include <dwmapi.h>
#include "skCrypt.h"

#pragma comment(lib, "d3d9.lib")

#include <string>
#include <functional>

// Data
static HWND                     gWindow = nullptr;
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

constexpr int   menuTabsCount = 4;

#define ICON_AIM      u8"\ue900"
#define ICON_VISUALS  u8"\ue901"
#define ICON_SETTINGS u8"\ue902"

static constexpr std::array menuIcons =
{
    ICON_AIM, ICON_VISUALS, ICON_SETTINGS
};

constexpr ImWchar iconsRanges[] = { 0xe900, 0xe902, 0 };

namespace gFonts
{
    namespace segoeui
    {
#include "Fonts/segoeui.hpp"
    }

    namespace framdit
    {
#include "Fonts/framdit.hpp"
    }

    namespace ntailu
    {
#include "Fonts/ntailu.hpp"
    }

    namespace icons
    {
#include "Fonts/icons.hpp"
    }

    static ImFont* iconsFont;
}

void styleColorsGold()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void InitStyle()
{
    auto& Style = ImGui::GetStyle();

    Style.WindowRounding = 12.000f;
    Style.ChildRounding = 12.000f;
    Style.PopupRounding = 12.000f;
    Style.FrameRounding = 12.000f;
    Style.FrameBorderSize = 1.000f;
    Style.GrabMinSize = 40.000f;
    Style.GrabRounding = 12.000f;
    Style.SelectableTextAlign = ImVec2(0.500f, 0.500f);
}

void updateMenuColorsStyle()
{
    switch (gGlobalVars->features.menu.colorsStyle)
    {
    case 0:
        styleColorsGold();
        break;
    case 1:
        ImGui::StyleColorsClassic();
        break;
    case 2:
        ImGui::StyleColorsDark();
        break;
    default:
        ImGui::StyleColorsLight();
        break;
    }
}

bool Menu::init(HINSTANCE instance)
{
    int          preference;
    HICON        icon2;
    WNDCLASSEX   windowClassEx;
    ImFontConfig fontConfig;
    auto         windowName = skCrypt("AK 8BP");

    icon2 = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
    windowClassEx.cbSize = sizeof(WNDCLASSEX);
    windowClassEx.style = CS_CLASSDC;
    windowClassEx.lpfnWndProc = WndProc;
    windowClassEx.cbClsExtra = 0;
    windowClassEx.cbWndExtra = 0;
    windowClassEx.hInstance = instance;
    windowClassEx.hIcon = icon2;
    windowClassEx.hCursor = nullptr;
    windowClassEx.hbrBackground = nullptr;
    windowClassEx.lpszMenuName = nullptr;
    windowClassEx.lpszClassName = "-";
    windowClassEx.hIconSm = nullptr;
    RegisterClassEx(&windowClassEx);

    gWindow = CreateWindow(
        "-",
        windowName.decrypt(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        100,
        100,
        570,
        500,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    windowName.clear();
    if (!CreateDeviceD3D(gWindow)) {
        CleanupDeviceD3D();
        UnregisterClass(windowClassEx.lpszClassName, windowClassEx.hInstance);
        return false;
    }

    ShowWindow(gWindow, SW_SHOWDEFAULT);
    UpdateWindow(gWindow);

    /* remove rounded coreners of my window in Win 11 */
    preference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(gWindow, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    updateMenuColorsStyle();
    //InitStyle();

    fontConfig.FontDataOwnedByAtlas = false;
    ImGui::GetIO().Fonts->AddFontFromMemoryTTF(gFonts::ntailu::rawData, sizeof(gFonts::ntailu::rawData), 20.f, &fontConfig);

    fontConfig = {};
    fontConfig.FontDataOwnedByAtlas = false;
    gFonts::iconsFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(gFonts::icons::rawData, sizeof(gFonts::icons::rawData), 50.f, 0, iconsRanges);

    ImGui_ImplWin32_Init(gWindow);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    return true;
}

void DrawTabs(int& currentTab, bool& shouldBreak);
void addPopupMenu(const char* popupName, std::function<void()> callBack, float offsetFromX = 0.f);

void Menu::runLoop()
{
    MSG                msg;
    int                currentTab;
    bool               shouldBreak;
    ImVec2             zeroImVec2{ 0.f, 0.f };
    HRESULT            result;
    constexpr D3DCOLOR directXClearColor = 4278190080UL;
    constexpr int      windowFlags = ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse;

    currentTab = 0;
    shouldBreak = false;
    while (shouldBreak == false) {
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                shouldBreak = true;
        }

        if (shouldBreak)
            break;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(zeroImVec2, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
        if (ImGui::Begin("", nullptr, windowFlags)) {
            DrawTabs(currentTab, shouldBreak);
            ImGui::End();
        }

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, directXClearColor, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }
}

void drawSeparator()
{
    float       itemSpacingY = ImGui::GetStyle().ItemSpacing.y;
    ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
    ImVec2      cursorScreenPos = ImGui::GetCursorScreenPos();

    cursorScreenPos.y += itemSpacingY;
    ImGui::SetCursorScreenPos(cursorScreenPos);
    windowDrawList->AddLine(ImVec2(cursorScreenPos.x - 9999, cursorScreenPos.y), ImVec2(cursorScreenPos.x + 9999, cursorScreenPos.y), ImGui::GetColorU32(ImGuiCol_Border));
    cursorScreenPos.y += itemSpacingY * 2.f;
    ImGui::SetCursorScreenPos(cursorScreenPos);
}

void addTooltip(const char* tip)
{
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltipEx(ImGuiTooltipFlags_OverridePreviousTooltip, ImGuiWindowFlags_None);
        ImGui::TextUnformatted(tip);
        ImGui::EndTooltip();
    }
}

static void aimbotTab()
{
    ImGui::Combo(skCrypt("aim action"), &gGlobalVars->features.automatic, skCrypt("none\0aim\0play\0"));
    if (gGlobalVars->features.automatic == 2) {
        ImGui::Text(skCrypt("power control"));
        addPopupMenu(
            skCrypt("power"),
            []() {
                ImGui::PushItemWidth(150.f);
                ImGui::Checkbox(skCrypt("humanized power"), &gGlobalVars->features.humanizedPower);
                if (gGlobalVars->features.humanizedPower) {
                    ImGui::SliderFloat(skCrypt("step size"), &gGlobalVars->features.powerStepSize, 0.05f, 0.2, "%.5f", ImGuiSliderFlags_AlwaysClamp);
                    addTooltip("keep it low for legit");
                }
                else
                {
                    ImGui::SliderFloat(skCrypt("play power"), &gGlobalVars->features.autoPlayMaxPower, 10.f, 100.f, "%.2f%%", ImGuiSliderFlags_AlwaysClamp);
                }

                ImGui::PopItemWidth();
            },
            160.f
                );

        ImGui::Text(skCrypt("angle rotation control"));
        addPopupMenu(
            skCrypt("angle rotation"),
            []() {
                ImGui::PushItemWidth(150.f);
                ImGui::Checkbox(skCrypt("humanized angle rotation"), &gGlobalVars->features.humanizedAngleRotation);
                if (gGlobalVars->features.humanizedAngleRotation) {
                    ImGui::SliderFloat(skCrypt("step size"), &gGlobalVars->features.angleRotationStepSize, 0.005f, 0.5f, "%.5f", ImGuiSliderFlags_AlwaysClamp);
                    addTooltip("keep it low for legit");
                    ImGui::SliderInt(skCrypt("delay between steps"), &gGlobalVars->features.delayBetweenAngleSteps, 1, 50, "%d ms", ImGuiSliderFlags_AlwaysClamp);
                }

                ImGui::PopItemWidth();
            },
            160.f
                );
    }

    ImGui::PushID(skCrypt("delay"));
    ImGui::Combo(skCrypt("delay mode"), &gGlobalVars->features.autoPlayDelayMode, skCrypt("custom\0random\0"));
    switch (gGlobalVars->features.autoPlayDelayMode)
    {
    case 0:
        ImGui::SliderFloat(skCrypt("delay"), &gGlobalVars->features.autoPlayDelay, 0.f, 15.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        break;
    default:
        ImGui::SliderFloat2(skCrypt("delay bounds"), &gGlobalVars->features.autoPlayDelayBounds[0], 0.f, 5.f, "%f", ImGuiSliderFlags_AlwaysClamp);
        break;
    }

    ImGui::PopID();

    /*ImGui::Checkbox("find best shot", &gGlobalVars->features.findBestShot);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("find shot with max balls could go in pockets\n");
    }*/
}

static void visualsTab()
{
    ImGui::Checkbox(skCrypt("draw prediction"), &gGlobalVars->features.esp.predicionPath);
    if (gGlobalVars->features.esp.predicionPath) {
        ImGui::PushID(skCrypt("predicionPath"));
        ImGui::SliderInt(skCrypt("prediction draw power"), &gGlobalVars->features.esp.displayPercantage, 10, 100, "%d%%");
        ImGui::SliderFloat(skCrypt("line thickness"), &gGlobalVars->features.esp.ballLineThickness, 1.f, 10.f);
        ImGui::SliderFloat(skCrypt("ball circle radius"), &gGlobalVars->features.esp.ballCircleRadius, 1.f, 40.f);
        ImGui::SliderFloat(skCrypt("ball transparency"), &gGlobalVars->features.esp.ballTransparency, 0.f, 100.f, "%.f%%");
        ImGui::Checkbox(skCrypt("ball circle filled"), &gGlobalVars->features.esp.ballCircleFilled);
        ImGui::PopID();
    }

    ImGui::Checkbox(skCrypt("draw shot state"), &gGlobalVars->features.esp.shotState);
    if (gGlobalVars->features.esp.shotState) {
        ImGui::PushID(skCrypt("shot state"));
        ImGui::SliderFloat(skCrypt("circle tickness"), &gGlobalVars->features.esp.shotStateCircleThickness, 1.f, 10.f);
        ImGui::SliderFloat(skCrypt("circle radius"), &gGlobalVars->features.esp.shotStateCircleRadius, 1.f, 50.f);
        ImGui::SliderFloat(skCrypt("circle transparency"), &gGlobalVars->features.esp.shotStateTransparency, 0.f, 100.f, "%.f%%");
        ImGui::Checkbox(skCrypt("circle filled"), &gGlobalVars->features.esp.shotStateCircleFilled);
        ImGui::PopID();
    }

    ImGui::Combo(skCrypt("prediction state"), &gGlobalVars->features.esp.state, skCrypt("player\0opponent\0both\0"));

    drawSeparator();

    if (ImGui::Checkbox(skCrypt("wide guideline"), &gGlobalVars->features.esp.wideGuideLine))
        UserSettings::setWideGuideLine(gGlobalVars, gGlobalVars->features.esp.wideGuideLine);

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip(skCrypt("requires rejoin.\n"));
}

static void miscTab(bool& shouldBreak)
{
    //ImGui::SliderInt("max win streak", &gGlobalVars->features.maxWinStreak, 3, 50, "%d");
    //if (ImGui::IsItemHovered()) {
    //    ImGui::SetTooltip("it forces you lose after you reach the max win streak.\nlower it to avoid shadow ban.\n");
    //}

    ImGui::Checkbox(skCrypt("adBlock"), &gGlobalVars->features.adBlock);
    drawSeparator();
    if (ImGui::Combo(skCrypt("colors style"), &gGlobalVars->features.menu.colorsStyle, skCrypt("gold\0classic\0dark\0light\0"))) {
        updateMenuColorsStyle();
    }

    if (ImGui::Button(skCrypt("load"), ImVec2(200.f, 0.f))) {
        Config::load();
        updateMenuColorsStyle();
    }

    if (ImGui::Button(skCrypt("save"), ImVec2(200.f, 0.f)))
        Config::save();

    if (ImGui::Button(skCrypt("unload"), ImVec2(200.f, 0.f)))
        shouldBreak = true;
}

void DrawTabs(int& currentTab, bool& shouldBreak)
{
    int   index;
    float imageButtonSize;

    const auto& io = ImGui::GetIO();
    const auto& style = ImGui::GetStyle();

    imageButtonSize = (io.DisplaySize.y / 3.f);
    imageButtonSize -= style.FramePadding.y * 3.f;
    imageButtonSize -= style.ItemSpacing.y * 2.f;

    ImGui::Columns(2, nullptr, true);
    ImGui::SetColumnWidth(0, imageButtonSize / 2.f + style.ItemSpacing.x * 2.f);
    ImGui::PushFont(gFonts::iconsFont);
    for (index = 0; index < 3; index++) {
        ImGui::PushID(reinterpret_cast<void*>(index));
        if (ImGui::Button(reinterpret_cast<const char*>(menuIcons[index]), ImVec2(imageButtonSize / 2.f, imageButtonSize))) {
            currentTab = index;
        }

        ImGui::PopID();
    }

    ImGui::PopFont();
    ImGui::NextColumn();
    ImGui::PushItemWidth(200.f);
    switch (currentTab)
    {
    case 0:
        aimbotTab();
        break;
    case 1:
        visualsTab();
        break;
    default:
        miscTab(shouldBreak);
        break;
    }

    ImGui::PopItemWidth();
    ImGui::Columns(1);
}

void addPopupMenu(const char* popupName, std::function<void()> callBack, float offsetFromX)
{
    ImGui::PushID(popupName);
    ImGui::SameLine(offsetFromX);
    if (ImGui::Button("...", { 50.f, 0.f })) {
        ImGui::OpenPopup(popupName);
    }

    if (ImGui::BeginPopup(popupName)) {
        callBack();
        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void Menu::end(HINSTANCE instance)
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(gWindow);
    UnregisterClass("-", instance);
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
