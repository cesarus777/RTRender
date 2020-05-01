#pragma once

#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

#include "geometry.hpp"
#include "obj_parser.hpp"

//#define USING_SDL

//#define FLUSH

void draw_line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2);

void draw_line(SDL_Renderer *renderer, vec2i v1, vec2i v2);

void draw_triangle(SDL_Renderer *renderer, vec2i v1, vec2i v2, vec2i v3);

void draw_triangle(SDL_Renderer *renderer, vec3i v1, vec3i v2, vec3i v3,
    double *zbuf, size_t xsize, size_t ysize);

void draw_triangle(SDL_Renderer *renderer, obj_model *model, vec3i v1, vec3i v2, vec3i v3,
    vec2i t1, vec2i t2, vec2i t3, double intensity, double *zbuf, size_t xsize, size_t ysize);

