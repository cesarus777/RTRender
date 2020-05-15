#pragma once

#include <string>

#include <SDL.h>

#include "exceptions.hpp"
#include "obj_parser.hpp"

enum { WIDTH = 800, HEIGHT = 800 };

class workspace_t
{
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  obj_model *model;
  double *zbuf;
  size_t width;
  size_t height;
public:
  workspace_t(size_t w = WIDTH, size_t h = HEIGHT)
  {
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      throw sdl_error(SDL_GetError());
    }

    width = w;
    height = h;

    if(SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) < 0)
    {
      width = height = 0;
      throw sdl_error(SDL_GetError());
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    zbuf = new double[width * height];
    for(size_t i = 0; i < height; ++i)
      for(size_t j = 0; j < width; ++j)
        zbuf[j + i * width] = -std::numeric_limits<double>::max();
  }
 
  workspace_t(std::string filename, size_t w = WIDTH, size_t h = HEIGHT)
  {
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      throw sdl_error(SDL_GetError());
    }

    width = w;
    height = h;

    if(SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) < 0)
    {
      width = height = 0;
      throw sdl_error(SDL_GetError());
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    try
    {
      model = new obj_model(filename);
    }
    catch(const std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
    }

    zbuf = new double[width * height];
    for(size_t i = 0; i < height; ++i)
      for(size_t j = 0; j < width; ++j)
        zbuf[j + i * width] = -std::numeric_limits<double>::max();
  }

  ~workspace_t()
  {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    delete model;
    delete [] zbuf;
  }

  size_t w_size() const { return width; }

  size_t h_size() const { return height; }

  const obj_model& obj() const { return *model; }

  void change_model(std::string filename)
  {
    delete model;
    try
    {
      model = new obj_model(filename);
    }
    catch(const std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
      model = nullptr;
    }
  }
  
  void update() const
  {
    SDL_RenderPresent(renderer);
  }

  void set_draw_color(int r, int g, int b) const
  {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  }

  void draw_line(int x1, int y1, int x2, int y2) const
  {
#ifdef USING_SDL
    if(SDL_RenderDrawLine(renderer, x1, y1, x2, y2) < 0)
      throw sdl_error(SDL_GetError());
#else
    if(x1 == x2)
    {
      if(y1 > y2)
        std::swap(y1, y2);
      for(int y = y1; y <= y2; ++y)
      {
        if(SDL_RenderDrawPoint(renderer, x1, y) < 0)
          throw sdl_error(SDL_GetError());
#ifdef FLUSH_SDL
        update();
#endif
      }
      return;
    }

    if(y1 == y2)
    {
      if(x1 > x2)
        std::swap(x1, x2);
      for(int x = x1; x <= x2; ++x)
      {
        if(SDL_RenderDrawPoint(renderer, x, y1) < 0)
          throw sdl_error(SDL_GetError());
#ifdef FLUSH_SDL
        update();
#endif
      }
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

#ifdef FLUSH_SDL
      update();
#endif

      error += derror;
      if(error > dx)
      {
        y += (dy > 0) ? 1 : -1;
        error -= dx * 2;
      }
    }
#endif
  }

  void draw_line(vec2i v1, vec2i v2)
  {
    draw_line(v1.x, v1.y, v2.x, v2.y);
  }

  void draw_triangle(triangle<int, 2> verts) const
  {
    if((verts[0].y == verts[1].y) && (verts[0].y == verts[2].y)) return;

    if(verts[0].y > verts[1].y)
      std::swap(verts[0], verts[1]);
    if(verts[0].y > verts[2].y)
      std::swap(verts[0], verts[2]);
    if(verts[1].y > verts[2].y)
      std::swap(verts[1], verts[2]);

    for(int y = verts[0].y; y < verts[2].y; ++y)
    {
      int x1 = verts[0].x + (y - verts[0].y) * (verts[2].x - verts[0].x) /
        static_cast<double>(verts[2].y - verts[0].y);
      int x2 = (y < verts[1].y)
        ? verts[0].x + (y - verts[0].y) * (verts[1].x - verts[0].x) /
            static_cast<double>(verts[1].y - verts[0].y)
        : verts[1].x + (y - verts[1].y) * (verts[2].x - verts[1].x) /
            static_cast<double>(verts[2].y - verts[1].y);
      draw_line(x1, y, x2, y);
    }
  }

  void draw_triangle(triangle<int, 3> verts) const
  {
    if(verts[0].y > verts[1].y)
      std::swap(verts[0], verts[1]);
    if(verts[0].y > verts[2].y)
      std::swap(verts[0], verts[2]);
    if(verts[1].y > verts[2].y)
      std::swap(verts[1], verts[2]);

    for(int y = verts[0].y; y <= verts[2].y; ++y)
    {
      bool second = (y >= verts[1].y);
      double k1 = (y - verts[0].y) / static_cast<double>(verts[2].y - verts[0].y);
      double k2 = (second)
        ? (y - verts[1].y) / static_cast<double>(verts[2].y - verts[1].y)
        : (y - verts[0].y) / static_cast<double>(verts[1].y - verts[0].y);

      vec3d whole = vec3d(verts[0]) + vec3d(verts[2] - verts[0]) * k1;
      vec3d compound = second
        ? vec3d(verts[1]) + vec3d(verts[2] - verts[1]) * k2
        : vec3d(verts[0]) + vec3d(verts[1] - verts[0]) * k2;

      if(whole.x > compound.x)
        std::swap(whole, compound);

      for(int x = whole.x; x <= compound.x; ++x)
      {
        double phi = (whole.x == compound.x)
          ? 1.0
          : (x - whole.x) / static_cast<double>(compound.x - whole.x);
        vec3d point = vec3d(whole) + vec3d(compound - whole) * phi;
        size_t i = point.y + point.x * width;
        if(zbuf[i] < point.z)
        {
          zbuf[i] = point.z;
          SDL_RenderDrawPoint(renderer, std::round(point.x), std::round(point.y));
#ifdef FLUSH
          update();
#endif
        }
      }
    }
  }

  void draw_triangle(triangle<int, 3> verts, triangle<int, 2> texture,
      double intensity) const
  {
    if(verts[0].y > verts[1].y)
    {
      std::swap(verts[0], verts[1]);
      std::swap(texture[0], texture[1]);
    }
    if(verts[0].y > verts[2].y)
    {
      std::swap(verts[0], verts[2]);
      std::swap(texture[0], texture[2]);
    }
    if(verts[1].y > verts[2].y)
    {
      std::swap(verts[1], verts[2]);
      std::swap(texture[1], texture[2]);
    }

    for(int y = verts[0].y; y <= verts[2].y; ++y)
    {
      bool second = (y >= verts[1].y);
      double k1 = (y - verts[0].y) / static_cast<double>(verts[2].y - verts[0].y);
      double k2 = (second)
        ? (y - verts[1].y) / static_cast<double>(verts[2].y - verts[1].y)
        : (y - verts[0].y) / static_cast<double>(verts[1].y - verts[0].y);

      vec3d whole = vec3d(verts[0]) + vec3d(verts[2] - verts[0]) * k1;
      vec3d compound = second
        ? vec3d(verts[1]) + vec3d(verts[2] - verts[1]) * k2
        : vec3d(verts[0]) + vec3d(verts[1] - verts[0]) * k2;

      vec2d t_whole = vec2d(texture[0]) + vec2d(texture[2] - texture[0]) * k1;
      vec2d t_compound = second
        ? vec2d(texture[1]) + vec2d(texture[2] - texture[1]) * k2
        : vec2d(texture[0]) + vec2d(texture[1] - texture[0]) * k2;

      if(whole.x > compound.x)
      {
        std::swap(whole, compound);
        std::swap(t_whole, t_compound);
      }

      for(int x = whole.x; x <= compound.x; ++x)
      {
        double phi = (whole.x == compound.x)
          ? 1.0
          : (x - whole.x) / static_cast<double>(compound.x - whole.x);
        vec3d point = whole + vec3d(compound - whole) * phi;
        vec2d t_point = t_whole + vec2d(t_compound - t_whole) * phi;
        size_t i = point.y + point.x * width;
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
          update();
#endif
        }
      }
    }
  }
};

