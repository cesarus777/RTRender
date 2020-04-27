#include <cstring>
#include <iostream>

#include <SDL.h>

#include "obj_parser.hpp"
#include "primitives.hpp"

enum { WIDTH = 800, HEIGHT = 800, MAX = 1 };

enum status_t { GOLD, NOT_GOLD, TRIANGLE, WIREFRAME, RAND, RAST, REMOV };

void render_wireframe(SDL_Renderer *renderer, obj_model *model)
{
  for(size_t i = 0; i < model->nfaces(); ++i)
  {
    auto face = model->face(i);
    for(size_t j = 0; j < 3; ++j)
    {
      vec3d v1 = model->vertice(face[j]);
      vec3d v2 = model->vertice(face[(j + 1) % 3]);
      int x1 = (v1.x + MAX) * WIDTH / (2.0 * MAX);
      int y1 = (MAX - v1.y) * HEIGHT / (2.0 * MAX);
      int x2 = (v2.x + MAX) * WIDTH / (2.0 * MAX);
      int y2 = (MAX - v2.y) * HEIGHT / (2.0 * MAX);
      draw_line(renderer, x1, y1, x2, y2);
#ifdef FLUSH
      SDL_RenderPresent(renderer);
#endif
    }
  }
}

void render_randcolor(SDL_Renderer *renderer, obj_model *model)
{
  for(size_t i = 0; i < model->nfaces(); ++i)
  {
    auto face = model->face(i);
    vec2i coords[3];
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model->vertice(face[j]);
      double x = (v.x + MAX) * WIDTH / (2 * MAX) + 0.5;
      double y = (MAX - v.y) * HEIGHT / (2 * MAX) + 0.5;
      coords[j] = vec2i(x, y);
    }
    int r = std::rand();
    char red = r % 256;
    char green = (r >> 8)  % 256;
    char blue = (r >> 16) % 256;
    char alpha = (r >> 24) % 256;
    SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
    draw_triangle(renderer, coords[0], coords[1], coords[2]);
#ifdef FLUSH
    SDL_RenderPresent(renderer);
#endif
  }
}

void render_rasterisator(SDL_Renderer *renderer, obj_model *model)
{
  vec3d light(.0, .0, -1.0);
  for(size_t i = 0; i < model->nfaces(); ++i)
  {
    auto face = model->face(i);
    vec2i coords[3];
    vec3d world[3];
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model->vertice(face[j]);
      int x = (v.x + MAX) * WIDTH / (2 * MAX);
      int y = (MAX - v.y) * HEIGHT / (2 * MAX);
      coords[j] = vec2i(x, y);
      world[j] = v;
    }
    vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
    n.normalize();
    double intensity = n * light;
    if(intensity > 0.0)
    {
      char red;
      char green;
      char blue;
      red = green = blue = intensity * 255;
      char alpha = static_cast<char>(0);
      SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
      draw_triangle(renderer, coords[0], coords[1], coords[2]);
#ifdef FLUSH
      SDL_RenderPresent(renderer);
#endif
    }
  }
}

void render_removal_rasterisator(SDL_Renderer *renderer, obj_model *model)
{
  vec3d light(.0, .0, -1.0);
  const size_t w = WIDTH;
  const size_t h = HEIGHT;
  double *zbuf = new double[w * h];
  for(size_t i = 0; i < h; ++i)
    for(size_t j = 0; j < w; ++j)
      zbuf[j + i * w] = -std::numeric_limits<double>::max();

  for(size_t i = 0; i < model->nfaces(); ++i)
  {
    auto face = model->face(i);
    vec3i coords[3];
    vec3d world[3];
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model->vertice(face[j]);
      int x = (v.x + MAX) * WIDTH / (2 * MAX);
      int y = (MAX - v.y) * HEIGHT / (2 * MAX);
      coords[j] = vec3i(x, y, v.z);
      world[j] = v;
    }
    vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
    n.normalize();
    double intensity = n * light;
    if(intensity > 0.0)
    {
      char red;
      char green;
      char blue;
      red = green = blue = intensity * 255;
      char alpha = static_cast<char>(0);
      SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
      draw_triangle(renderer, coords[0], coords[1], coords[2], zbuf, w, h);
#ifdef FLUSH
      SDL_RenderPresent(renderer);
#endif
    }
  }
}

void render_lines(SDL_Renderer *renderer, status_t status)
{
  SDL_RenderDrawPoint(renderer, WIDTH / 2, HEIGHT / 2);

  bool ok = true;
  int fib = 1, prev_fib = 0;
  int new_x = WIDTH / 2, new_y = HEIGHT / 2,
      prev_x = WIDTH / 2, prev_y = HEIGHT / 2;
  bool gold = (status == GOLD);

  while(ok)
  {
    draw_line(renderer, prev_x, prev_y, new_x, new_y);
#ifdef FLUSH
    SDL_RenderPresent(renderer);
#endif

    if(gold)
    {
    fib += prev_fib;
    prev_fib = fib;
    }
    else
    {
      prev_fib = fib++;
    }
    prev_x = new_x;
    prev_y = new_y;
    static int i = 0;
    switch(i++)
    {
      case 0:
        new_x = prev_x + fib;
        new_y = prev_y - fib;
        break;
      case 1:
        new_x = prev_x + fib;
        new_y = prev_y + fib;
        break;
      case 2:
        new_x = prev_x - fib;
        new_y = prev_y + fib;
        break;
      case 3:
        new_x = prev_x - fib;
        new_y = prev_y - fib;
        break;
    }

    i %= 4;

    if(((new_x < 0) || (new_x > WIDTH)) || ((new_y < 0) || (new_y > HEIGHT)))
    {
      if(new_x < 0)
      {
        new_y = new_y + new_x * (new_y - prev_y) / (double) (new_x - prev_x);
        new_x = 0;
      }
      if(new_x > WIDTH)
      {
        new_y = new_y - (new_x - WIDTH) * (new_y - prev_y) / (double) (new_x - prev_x);
        new_x = WIDTH;
      }
      if(new_y < 0)
      {
        new_x = new_x + new_y * (new_x - prev_x) / (double) (new_y - prev_y);
        new_y = 0;
      }
      if(new_y > HEIGHT)
      {
        new_x = new_x - (new_y - HEIGHT) * (new_x - prev_x) / (double) (new_y - prev_y);
        new_y = HEIGHT;
      }

      draw_line(renderer, prev_x, prev_y, new_x, new_y);

      ok = false;
    }
  }
}

void render_triangles(SDL_Renderer *renderer)
{
  const size_t w = WIDTH;
  const size_t h = HEIGHT;
  double *zbuf = new double[w * h];
  for(size_t i = 0; i < w * h; ++i)
    zbuf[i] = -std::numeric_limits<double>::max();

  vec3i v1(100, 400, -10);
  vec3i v2(700, 250, -1);
  vec3i v3(700, 550, -1);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  draw_triangle(renderer, v1, v2, v3, zbuf, w, h);
  v1 = vec3i(300, 100, -5);
  v2 = vec3i(300, 700, -5);
  v3 = vec3i(525, 400, -1);
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  draw_triangle(renderer, v1, v2, v3, zbuf, w, h);
  v1 = vec3i(600, 50, -5);
  v2 = vec3i(600, 750, -5);
  v3 = vec3i(475, 400, -1);
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  draw_triangle(renderer, v1, v2, v3, zbuf, w, h);
  delete [] zbuf;
}

int main(int argc, char **argv)
{
  status_t status;
  obj_model *model = NULL;
  if(argc == 1)
  {
    std::cerr << "argc == 1" << std::endl;
    exit(EXIT_FAILURE);
  }
  if(argc == 2)  
  {
    if(strcmp(argv[1], "-ng") == 0)
    {
      status = NOT_GOLD;
    }
    else if(strcmp(argv[1], "-g") == 0)
    {
      status = GOLD;
    }
    else if(strcmp(argv[1], "-t") == 0)
    {
      status = TRIANGLE;
    }
    else
    {
      std::cerr << "unknown arg" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  if(argc == 3)
  {
    if(strcmp(argv[1], "-wire") == 0)
    {
      status = WIREFRAME;
    }
    else if(strcmp(argv[1], "-rand") == 0)
    {
      status = RAND;
    }
    else if(strcmp(argv[1], "-rast") == 0)
    {
      status = RAST;
    }
    else if(strcmp(argv[1], "-remov") == 0)
    {
      status = REMOV;
    }
    else
    {
      std::cerr << "unknown args" << std::endl;
      exit(EXIT_FAILURE);
    }
    model = new obj_model(argv[2]);
  }
  if(argc > 4)
  {
    std::cerr << "too many args" << std::endl;
    exit(EXIT_FAILURE);
  }

  SDL_Window *window;
  SDL_Renderer *renderer;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  switch(status)
  {
    case WIREFRAME:
      render_wireframe(renderer, model);
      break;
    case RAND:
      render_randcolor(renderer, model);
      break;
    case RAST:
      render_rasterisator(renderer, model);
      break;
    case REMOV:
      render_removal_rasterisator(renderer, model);
      break;
    case TRIANGLE:
      render_triangles(renderer);
      break;
    default:
      render_lines(renderer, status);
      break;
  }

  SDL_RenderPresent(renderer);

  SDL_Event event;
  for(;;)
  {
    SDL_WaitEvent(&event);
    if((event.type == SDL_QUIT) || (event.type == SDL_KEYDOWN))
      break;
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}

