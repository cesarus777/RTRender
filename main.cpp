#include <cstring>
#include <iostream>
#include <string>
#include <variant>

#include <SDL.h>

#include "exceptions.hpp"
#include "obj_parser.hpp"
#include "renderer.hpp"

enum mode_type { EMPTY, GOLD, NOT_GOLD, TRIANGLE, WIREFRAME, RAND, RAST, REMOV, COLOR };

enum arg_t { UNKNOWN, WITHOUT_FILE, WITH_FILE, SIZE };

struct arg_status_t
{
  std::variant<mode_type, size_t> info;
  arg_t arg;
};

void render_wireframe(const workspace_t &workspace)
{
  const obj_model &model = workspace.obj();
  double max = model.max();
  double xshift = model.xshift();
  double yshift = model.yshift();
  size_t width = workspace.w_size();
  size_t height = workspace.h_size();
  for(size_t i = 0; i < model.nfaces(); ++i)
  {
    auto face = model.face(i);
    for(size_t j = 0; j < 3; ++j)
    {
      vec3d v1 = model.vertice(face[j]);
      vec3d v2 = model.vertice(face[(j + 1) % 3]);
      int x1 = (max + xshift + v1.x) * width / (2.0 * max);
      int y1 = (max + yshift - v1.y) * height / (2.0 * max);
      int x2 = (max + xshift + v2.x) * width / (2.0 * max);
      int y2 = (max + yshift - v2.y) * height / (2.0 * max);
      workspace.draw_line(x1, y1, x2, y2);
#ifdef FLUSH
      workspace.update();
#endif
    }
  }
}

void render_randcolor(const workspace_t& workspace)
{
  const obj_model &model = workspace.obj();
  double max = model.max();
  double xshift = model.xshift();
  double yshift = model.yshift();
  size_t width = workspace.w_size();
  size_t height = workspace.h_size();
  for(size_t i = 0; i < model.nfaces(); ++i)
  {
    auto face = model.face(i);
    triangle<int, 2> coords;
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model.vertice(face[j]);
      double x = (max + xshift + v.x) * width / (2 * max);
      double y = (max + yshift - v.y) * height / (2 * max);
      coords[j] = vec2i(x, y);
    }
    int r = std::rand();
    char red = r % 256;
    char green = (r >> 8)  % 256;
    char blue = (r >> 16) % 256;
    workspace.set_draw_color(red, green, blue);
    workspace.draw_triangle(coords);
#ifdef FLUSH
    workspace.update();
#endif
  }
}

void render_rasterisator(const workspace_t& workspace)
{
  vec3d light(0.0, 0.0, -1.0);
  const obj_model &model = workspace.obj();
  double max = model.max();
  double xshift = model.xshift();
  double yshift = model.yshift();
  size_t width = workspace.w_size();
  size_t height = workspace.h_size();
  for(size_t i = 0; i < model.nfaces(); ++i)
  {
    auto face = model.face(i);
    triangle<int, 2> coords;
    triangle<double, 3> world;
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model.vertice(face[j]);
      int x = (max + xshift + v.x) * width / (2 * max);
      int y = (max + yshift - v.y) * height / (2 * max);
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
      workspace.set_draw_color(red, green, blue);
      workspace.draw_triangle(coords);
#ifdef FLUSH
      workspace.update();
#endif
    }
  }
}

void render_removal_rasterisator(const workspace_t& workspace)
{
  vec3d light(0.0, 0.0, -1.0);
  const obj_model &model = workspace.obj();
  double max = model.max();
  double xshift = model.xshift();
  double yshift = model.yshift();
  size_t width = workspace.w_size();
  size_t height = workspace.h_size();
  for(size_t i = 0; i < model.nfaces(); ++i)
  {
    auto face = model.face(i);
    triangle<int, 3> coords;
    triangle<double, 3> world;
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model.vertice(face[j]);
      int x = (max + xshift + v.x) * width / (2 * max);
      int y = (max + yshift - v.y) * height / (2 * max);
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
      workspace.set_draw_color(red, green, blue);
      workspace.draw_triangle(coords);
#ifdef FLUSH
      workspace.update();
#endif
    }
  }
}

void render_color_removal(const workspace_t& workspace)
{
  vec3d light(0.0, 0.0, -1.0);
  const obj_model &model = workspace.obj();
  double max = model.max();
  double xshift = model.xshift();
  double yshift = model.yshift();
  size_t width = workspace.w_size();
  size_t height = workspace.h_size();

  for(size_t i = 0; i < model.nfaces(); ++i)
  {
    auto face = model.face(i);
    triangle<int, 3> coords;
    triangle<double, 3> world;
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model.vertice(face[j]);
      int x = (max + xshift + v.x) * width / (2 * max);
      int y = (max + yshift - v.y) * height / (2 * max);
      coords[j] = vec3i(x, y, v.z);
      world[j] = v;
    }
    vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
    n.normalize();
    double intensity = n * light;
    if(intensity > 0.0)
    {
      triangle<int, 2> tv;
      for(size_t k = 0; k < 3; ++k)
      {
        tv[k] = model.tv(i, k);
      }
      workspace.draw_triangle(coords, tv, intensity);
#ifdef FLUSH
      workspace.update();
#endif
    }
  }
}

void render_lines(const workspace_t& workspace, mode_type mode)
{
  bool ok = true;
  int fib = 1, prev_fib = 0;
  int width = workspace.w_size();
  int height = workspace.h_size();
  int prev_x, new_x = prev_x = width / 2,
      prev_y, new_y = prev_y = height / 2;
  bool gold = (mode == GOLD);

  while(ok)
  {
    workspace.draw_line(prev_x, prev_y, new_x, new_y);

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

    if(((new_x < 0) || (new_x > width)) || ((new_y < 0) || (new_y > height)))
    {
      if(new_x < 0)
      {
        new_y = new_y + new_x * (new_y - prev_y) /
          static_cast<double>(new_x - prev_x);
        new_x = 0;
      }
      if(new_x > width)
      {
        new_y = new_y - (new_x - width) * (new_y - prev_y) /
          static_cast<double>(new_x - prev_x);
        new_x = width;
      }
      if(new_y < 0)
      {
        new_x = new_x + new_y * (new_x - prev_x) /
          static_cast<double>(new_y - prev_y);
        new_y = 0;
      }
      if(new_y > height)
      {
        new_x = new_x - (new_y - height) * (new_x - prev_x) /
          static_cast<double>(new_y - prev_y);
        new_y = height;
      }

      workspace.draw_line(prev_x, prev_y, new_x, new_y);

      ok = false;
    }
  }
}

void render_triangles(const workspace_t& workspace)
{
  vec3i v1(100, 400, -10);
  vec3i v2(700, 250, -1);
  vec3i v3(700, 550, -1);
  workspace.set_draw_color(0xff, 0x00, 0x00);
  triangle<int, 3> t1(v1, v2, v3);
  workspace.draw_triangle(t1);
  v1 = vec3i(300, 100, -5);
  v2 = vec3i(300, 700, -5);
  v3 = vec3i(525, 400, -1);
  workspace.set_draw_color(0x00, 0xff, 0x00);
  triangle<int, 3> t2(v1, v2, v3);
  workspace.draw_triangle(t2);
  v1 = vec3i(600, 50, -5);
  v2 = vec3i(600, 750, -5);
  v3 = vec3i(475, 400, -1);
  workspace.set_draw_color(0x00, 0x00, 0xff);
  triangle<int, 3> t3(v1, v2, v3);
  workspace.draw_triangle(t3);
}

void main_init(int argc, char **argv, mode_type *mode, workspace_t **workspace);

int main(int argc, char **argv)
{
  mode_type mode = EMPTY;
  workspace_t *workspace = nullptr;
  main_init(argc, argv, &mode, &workspace);

  workspace->set_draw_color(0xff, 0xff, 0xff);

  switch(mode)
  {
    case WIREFRAME:
      render_wireframe(*workspace);
      break;
    case RAND:
      render_randcolor(*workspace);
      break;
    case RAST:
      render_rasterisator(*workspace);
      break;
    case REMOV:
      render_removal_rasterisator(*workspace);
      break;
    case COLOR:
      render_color_removal(*workspace);
      break;
    case TRIANGLE:
      render_triangles(*workspace);
      break;
    case EMPTY:
      break;
    default:
      render_lines(*workspace, mode);
      break;
  }

  workspace->update();

  SDL_Event event;
  for(;;)
  {
    SDL_WaitEvent(&event);
    if((event.type == SDL_QUIT) || (event.type == SDL_KEYDOWN))
      break;
  }

  return EXIT_SUCCESS;
}

arg_status_t arg_parser(const char *argv)
{
  arg_status_t result;
  if(strcmp(argv, "-ng") == 0)
  {
    result.info = NOT_GOLD;
    result.arg = WITHOUT_FILE;
  }
  else if(strcmp(argv, "-g") == 0)
  {
    result.info = GOLD;
    result.arg = WITHOUT_FILE;
  }
  else if(strcmp(argv, "-t") == 0)
  {
    result.info = TRIANGLE;
    result.arg = WITHOUT_FILE;
  }
  else if(strcmp(argv, "-wire") == 0)
  {
    result.info = WIREFRAME;
    result.arg = WITH_FILE;
  }
  else if(strcmp(argv, "-rand") == 0)
  {
    result.info = RAND;
    result.arg = WITH_FILE;
  }
  else if(strcmp(argv, "-rast") == 0)
  {
    result.info = RAST;
    result.arg = WITH_FILE;
  }
  else if(strcmp(argv, "-remov") == 0)
  {
    result.info = REMOV;
    result.arg = WITH_FILE;
  }
  else if(strcmp(argv, "-color") == 0)
  {
    result.info = COLOR;
    result.arg = WITH_FILE;
  }
  else if(strcmp(argv, "-size=") > 0)
  {
    result.arg = SIZE;
    int size = strtol(argv + 6, nullptr, 10);
    result.info = static_cast<size_t>(size);
  }
  else
  {
    result.arg = UNKNOWN;
    result.info = EMPTY;
  }
  return result;
}

void main_init(int argc, char **argv, mode_type *mode, workspace_t **workspace)
{
  if(argc == 1)
  {
    std::cerr << "Too few args!" << std::endl;
    exit(EXIT_FAILURE);
  }

  size_t size = 0;
  std::string filename;

  for(int i = 1; i < argc; ++i)
  {
    if(argv[i][0] == '-')
    {
      arg_status_t arg_status = arg_parser(argv[i]);

      if(arg_status.arg == SIZE)
      {
        size = std::get<size_t>(arg_status.info);
      }

      if(arg_status.arg == UNKNOWN)
      {
        std::cerr << "Unknown argument!" << std::endl;
        exit(EXIT_FAILURE);
      }

      if(arg_status.arg == WITHOUT_FILE)
      {
        if(*mode == EMPTY)
        {
          *mode = std::get<mode_type>(arg_status.info);
        }
        else
        {
          std::cerr << "Can't run with 2 mods" << std::endl;
        }
      }

      if(arg_status.arg == WITH_FILE)
      {
        if((*mode == EMPTY) && ((argc - i) > 0))
        {
          *mode = std::get<mode_type>(arg_status.info);
          filename = argv[++i];
        }
        else
        {
          std::cerr << "Can't run with 2 mods" << std::endl;
        }
      }
    }
    else
    {
      std::cerr << "Unknown argument!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  if(filename.size())
  {
    try
    {
      if(size != 0)
        *workspace = new workspace_t(filename, size, size);
      else
        *workspace = new workspace_t(filename);
    }
    catch(const sdl_error& e)
    {
      std::cerr << "Can't create workspace : " << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    try
    {
      if(size != 0)
        *workspace = new workspace_t(size, size);
      else
        *workspace = new workspace_t();
    }
    catch(const sdl_error& e)
    {
      std::cerr << "Can't create workspace : " << e.what() << std::endl;
    }
  }

  if(workspace == nullptr)
  {
    exit(EXIT_FAILURE);
  }
}

