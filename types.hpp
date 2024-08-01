#pragma once

#include <iostream>
#include <stdio.h>
#include <array>

struct Texture
{
    float width, height;
    unsigned int ID;
};

typedef struct
{
    float x, y;
} Vector2;

struct Vector3
{
    float x, y, z;
};

struct Color
{
    float R, G, B, A;
};

struct Vertex
{
    Vector3 Position;
    Color Color;
    Vector2 TexCoords;
    float TexID;
};

typedef struct
{
    float x, y, w, h;
} Rectangle;