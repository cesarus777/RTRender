#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "geometry.hpp"

class tga_image
{
private:
  SDL_Surface *image;
public:
  tga_image() : image(nullptr) {}
  tga_image(const char* filename)
  {
    SDL_RWops *rwop;
    rwop = SDL_RWFromFile(filename, "rb");
    image = IMG_LoadTGA_RW(rwop);
    SDL_RWclose(rwop);
    if(!image)
      throw std::runtime_error("can't open " + std::string(filename));
  }

  ~tga_image()
  {
    if(image)
      SDL_FreeSurface(image);
  }

  size_t width() const { return image->w; }
  size_t height() const { return image->h; }

  void read_tga(const char* filename)
  {
    SDL_RWops *rwop;
    rwop = SDL_RWFromFile(filename, "rb");
    image = IMG_LoadTGA_RW(rwop);
    SDL_RWclose(rwop);
    if(!image)
      throw std::runtime_error("can't open " + std::string(filename));
  }

  SDL_Color pixel_color(int x, int y)
  {
    if((x >= image->w) || (y >= image->h))
      throw std::out_of_range("searching for a pixel out of surface");

    y = image->h - y;
    int bpp = image->format->BytesPerPixel;
    Uint8 *ptr = (Uint8 *)image->pixels + y * image->pitch + x * bpp;

    unsigned p;
    switch (bpp)
    {
      case 1:
        p = *ptr;
        break;

      case 2:
        p = *(Uint16*)ptr;
        break;

      case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
          p = (ptr[0] << 16 | ptr[1] << 8 | ptr[2]);
        else
          p = (ptr[0] | ptr[1] << 8 | ptr[2] << 16);
        break;

      case 4:
        p = *(Uint32*)ptr;
        break;

      default:
        throw std::runtime_error("unknown pixel format : strange bpp");
    }

    SDL_Color clr;
    SDL_GetRGB(p, image->format, &clr.r, &clr.g, &clr.b);
    return clr;
  }
};

class obj_model
{
private:
  std::vector<vec3d> vertices;
  double max_x;
  double min_x;
  double max_y;
  double min_y;
  std::vector<std::vector<vec3i>> faces;
  std::vector<vec2d> texture_verts;
  tga_image diffuse;
public:
  obj_model(std::string filename)
  {
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::in);
    if(ifs.fail())
      throw std::runtime_error("Can't open model obj file");
    while(ifs)
    {
      std::string str;
      ifs >> str;
      if(str == "v")
      {
        vec3d v;
        for(size_t i = 0; i < 3; ++i)
          ifs >> v[i];
        if(v[0] > max_x)
          max_x = v[0];
        if(v[0] < min_x)
          min_x = v[0];
        if(v[1] > max_y)
          max_y = v[1];
        if(v[1] < min_y)
          min_y = v[1];
        vertices.push_back(v);
      }
      else if(str == "vt")
      {
        vec2d tmp;
        ifs >> tmp[0] >> tmp[1];
        texture_verts.push_back(tmp);
      }
      else if(str == "vn")
      {
        ifs >> str >> str >> str;
      }
      else if(str == "f")
      {
        std::vector<vec3i> f;
        vec3i tmp;

        for(size_t i = 0; i < 3; ++i)
        {
          ifs >> str;
          char *end1;
          char *end2;
          tmp[0] = std::strtol(str.data(), &end1, 10);
          if(end1[0] == '/')
            tmp[1] = std::strtol(end1 + 1, &end2, 10);
          if(end2[0] == '/')
            tmp[2] = std::strtol(end2 + 1, &end1, 10);
          for(size_t i = 0; i < 3; ++i)
            tmp[i]--;
          f.push_back(tmp);
        }

        faces.push_back(f);
      }
    }
    IMG_Init(0);
    auto iter = filename.rbegin();
    for(; iter != filename.rend(); ++iter)
    {
      if(*iter == '.')
        break;
    }
    std::string diff_name(filename.begin(), --iter.base());
    diff_name += "_diffuse.tga";
    try
    {
      diffuse.read_tga(diff_name.data());
    }
    catch(const std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
    }
  }

  ~obj_model()
  {
    IMG_Quit();
  }

  size_t nvertices() const { return vertices.size(); }

  size_t nfaces() const { return faces.size(); }

  double xshift() const { return (max_x + min_x) / 2; }
  double yshift() const { return (max_y + min_y) / 2; }

  double xborder() const { return (max_x - min_x) / 2; }
  double yborder() const { return (max_y - min_y) / 2; }

  double max() const
  {
    double x = std::max(max_x, -min_x);
    double y = std::max(max_y, -min_y);
    return std::max(x, y);
  }

  vec3d vertice(size_t i) const { return vertices[i]; }

  std::vector<int> face(size_t i) const
  {
    std::vector<int> res;
    for(const auto& elem : faces[i])
      res.push_back(elem.x);
    return res;
  }

  vec2i tv(size_t nface, size_t nvert) const
  {
    size_t i = faces[nface][nvert].y;
    return vec2i(texture_verts[i].x * diffuse.width(),
                 texture_verts[i].y * diffuse.height());
  }

  SDL_Color tv_clr(size_t nface, size_t nvert)
  {
    vec2i t = tv(nface, nvert);
    SDL_Color color;
    try
    {
      color = diffuse.pixel_color(t.x, t.y);
    }
    catch(const std::out_of_range& e)
    {
      std::cerr << e.what() << std::endl;
      color = { 0x00, 0x00, 0x00, 0xff };
    }
    catch(const std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
      color = { 0x00, 0x00, 0x00, 0xff };
    }
    return color;
  }

  SDL_Color tv_clr(int x, int y)
  {
    SDL_Color color;
    try
    {
      color = diffuse.pixel_color(x, y);
    }
    catch(const std::out_of_range& e)
    {
      std::cerr << e.what() << std::endl;
      color = { 0x00, 0x00, 0x00, 0xff };
    }
    catch(const std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
      color = { 0x00, 0x00, 0x00, 0xff };
    }
    return color;
  }
};

