
#include <array>
#include <Windows.h>

struct Color
{
    constexpr Color() { r = g = b = 1.f;  }
    constexpr Color(float red, float green, float blue)
    {
        r = red;
        g = green;
        b = blue;
    }

    float r, g, b;
};

std::array colors = std::to_array<Color>(
    {
        Color(1.f, 1.f, 1.f),                         // white
        Color(1.f, 1.f, 0.f),                         // yellow
        Color(0.f, 0.f, 1.f),                         // blue
        Color(1.f, 0.f, 0.f),                         // red
        Color(0.501960784314f, 0.f, 0.501960784314f), // purple
        Color(1.f, 0.647058823529f, 0.f),             // orange
        Color(0.f, 0.501960784314f, 0.f),             // green
        Color(0.501960784314f, 0.f, 0.f),             // maroon
        Color(0.f, 0.f, 0.f),                         // black
        Color(1.f, 1.f, 0.f),                         // yellow
        Color(0.f, 0.f, 1.f),                         // blue
        Color(1.f, 0.f, 0.f),                         // red
        Color(0.501960784314f, 0.f, 0.501960784314f), // purple
        Color(1.f, 0.647058823529f, 0.f),             // orange
        Color(0.f, 0.501960784314f, 0.f),             // green
        Color(0.501960784314f, 0.f, 0.f),             // maroon
    }
);

static_assert(colors.size() == 16);