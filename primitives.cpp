#include "primitives.hpp"

void draw_line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2)
{
#ifdef USING_SDL
  SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
#else
  if(x1 == x2)
  {
    if(y1 > y2)
      std::swap(y1, y2);
    for(int y = y1; y <= y2; ++y)
      SDL_RenderDrawPoint(renderer, x1, y);
    return;
  }

  bool transposed = (std::abs(y2 - y1) > std::abs(x2 - x1));
  if(transposed)
  {
    std::swap(x1, y1);
    std::swap(x2, y2);
  }

  if(x2 < x1)
  {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }

  int dx = x2 - x1;
  int dy = y2 - y1;
  int derror = std::abs(dy) * 2;
  int error = 0;
  int y = y1;
  for(int x = x1; x <= x2; ++x)
  {
    transposed
      ? SDL_RenderDrawPoint(renderer, y, x)
      : SDL_RenderDrawPoint(renderer, x, y);

    error += derror;
    if(error > dx)
    {
      y += (dy > 0) ? 1 : -1;
      error -= dx * 2;
    }
  }
#endif
}

void draw_line(SDL_Renderer *renderer, vec2i v1, vec2i v2)
{
  draw_line(renderer, v1.x, v1.y, v2.x, v2.y);
}

void draw_triangle(SDL_Renderer *renderer, vec2i v1, vec2i v2, vec2i v3)
{
  if((v1.y == v2.y) && (v1.y == v3.y)) return;

  if(v1.y > v2.y)
    std::swap(v1, v2);
  if(v1.y > v3.y)
    std::swap(v1, v3);
  if(v2.y > v3.y)
    std::swap(v2, v3);

  for(int y = v1.y; y < v3.y; ++y)
  {
    int x1 = v1.x + (y - v1.y) * (v3.x - v1.x) / (double) (v3.y - v1.y);
    int x2 = (y < v2.y)
      ? v1.x + (y - v1.y) * (v2.x - v1.x) / (double) (v2.y - v1.y)
      : v2.x + (y - v2.y) * (v3.x - v2.x) / (double) (v3.y - v2.y);
    draw_line(renderer, x1, y, x2, y);
  }
}

void draw_triangle(SDL_Renderer *renderer, vec3i v1, vec3i v2, vec3i v3,
    double *zbuf, size_t xsize, size_t ysize)
{
  if(xsize != ysize)
    ysize = xsize;

  if(v1.y > v2.y)
    std::swap(v1, v2);
  if(v1.y > v3.y)
    std::swap(v1, v3);
  if(v2.y > v3.y)
    std::swap(v2, v3);

  for(int y = v1.y; y <= v3.y; ++y)
  {
    bool second = (y >= v2.y);
    double k1 = (y - v1.y) / (double) (v3.y - v1.y);
    double k2 = (second)
      ? (y - v2.y) / (double) (v3.y - v2.y)
      : (y - v1.y) / (double) (v2.y - v1.y);

    vec3d whole = vec3d(v1) + vec3d(v3 - v1) * k1;
    vec3d compound = second
      ? vec3d(v2) + vec3d(v3 - v2) * k2
      : vec3d(v1) + vec3d(v2 - v1) * k2;

    if(whole.x > compound.x)
      std::swap(whole, compound);

    for(int x = whole.x; x <= compound.x; ++x)
    {
      double phi = (whole.x == compound.x)
        ? 1.0 : (x - whole.x) / (double) (compound.x - whole.x);
      vec3d point = vec3d(whole) + vec3d(compound - whole) * phi;
      size_t i = point.y + point.x * xsize;
      if(zbuf[i] < point.z)
      {
        zbuf[i] = point.z;
        SDL_RenderDrawPoint(renderer, std::round(point.x), std::round(point.y));
#ifdef FLUSH
        SDL_RenderPresent(renderer);
#endif
      }
    }
  }
}

void draw_triangle(SDL_Renderer *renderer, obj_model *model, vec3i v1, vec3i v2, vec3i v3,
    vec2i t1, vec2i t2, vec2i t3, double intensity, double *zbuf, size_t xsize, size_t ysize)
{
  if(xsize != ysize)
    ysize = xsize;

  if(v1.y > v2.y)
  {
    std::swap(v1, v2);
    std::swap(t1, t2);
  }
  if(v1.y > v3.y)
  {
    std::swap(v1, v3);
    std::swap(t1, t3);
  }
  if(v2.y > v3.y)
  {
    std::swap(v2, v3);
    std::swap(t2, t3);
  }

  for(int y = v1.y; y <= v3.y; ++y)
  {
    bool second = (y >= v2.y);
    double k1 = (y - v1.y) / (double) (v3.y - v1.y);
    double k2 = (second)
      ? (y - v2.y) / (double) (v3.y - v2.y)
      : (y - v1.y) / (double) (v2.y - v1.y);

    vec3d whole = vec3d(v1) + vec3d(v3 - v1) * k1;
    vec3d compound = second
      ? vec3d(v2) + vec3d(v3 - v2) * k2
      : vec3d(v1) + vec3d(v2 - v1) * k2;

    vec2d t_whole = vec2d(t1) + vec2d(t3 - t1) * k1;
    vec2d t_compound = second
      ? vec2d(t2) + vec2d(t3 - t2) * k2
      : vec2d(t1) + vec2d(t2 - t1) * k2;

    if(whole.x > compound.x)
    {
      std::swap(whole, compound);
      std::swap(t_whole, t_compound);
    }

    for(int x = whole.x; x <= compound.x; ++x)
    {
      double phi = (whole.x == compound.x)
        ? 1.0 : (x - whole.x) / (double) (compound.x - whole.x);
      vec3d point = whole + vec3d(compound - whole) * phi;
      vec2d t_point = t_whole + vec2d(t_compound - t_whole) * phi;
      size_t i = point.y + point.x * xsize;
      if(zbuf[i] < point.z)
      {
        zbuf[i] = point.z;
        SDL_Color clr = model->tv_clr((int)std::round(t_point[0]), (int)std::round(t_point[1]));
        char r = clr.r * intensity;
        char g = clr.g * intensity;
        char b = clr.b * intensity;
        char a = clr.a * intensity;
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderDrawPoint(renderer, std::round(point.x), std::round(point.y));
#ifdef FLUSH
        SDL_RenderPresent(renderer);
#endif
      }
    }
  }
}

