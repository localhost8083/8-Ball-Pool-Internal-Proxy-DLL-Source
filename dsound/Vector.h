#pragma once

#include "const.h"

#include <cfloat>
#include <Windows.h>

struct Vector2D
{
    Vector2D();
    Vector2D(vec_t dstX, vec_t dstY);

    bool     isNull() const;
    vec_t    length() const;
    Vector2D toScreen() const;
    void     print(const char* prefix = "", const char* suffix = "\n") const;
    vec_t    getDistanceTo(const Vector2D& destination) const;
    void     nullify();

    vec_t&   operator[](int i);
    vec_t&   operator[](int i) const;
    Vector2D operator-(const Vector2D& other);
    Vector2D operator-(const Vector2D& other) const;
    bool     operator!=(const Vector2D& other);
    void     operator=(const Vector2D& other);

    vec_t x, y;
};

struct Vector3D
{
    Vector3D();

    bool  isNull() const;
    vec_t length() const;
    void  print(const char* prefix = "", const char* suffix = "\n") const;

    inline void nullify() { x = y = z = vecZero; }

    inline vec_t& operator[](int i) { return PVECTOR(this)[i]; }
    inline vec_t& operator[](int i) const { return PVECTOR(this)[i]; }

    vec_t x, y, z;
};

struct Vector4D
{
    Vector4D();
    Vector4D(vec_t srcX, vec_t srcY, vec_t srcZ, vec_t srcW);

    vec_t x, y, z, w;
};