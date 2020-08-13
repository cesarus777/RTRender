#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "geometry.hpp"



class tga_image
{
private:
  SDL_Surface *image = nullptr;

public:
  tga_image() {}

  tga_image(const std::filesystem::path &filename)
  {
    read_tga(filename);
  }

  ~tga_image()
  {
    if(image != nullptr)
      SDL_FreeSurface(image);
  }

  inline size_t width() const noexcept { return image->w; }
  inline size_t height() const noexcept { return image->h; }

  void read_tga(const std::filesystem::path &filename)
  {
    if(image != nullptr)
      SDL_FreeSurface(image);

    SDL_RWops *rwop;
    rwop = SDL_RWFromFile(filename.c_str(), "rb");
    if (rwop == nullptr)
        throw no_file();

    image = IMG_LoadTGA_RW(rwop);   
    SDL_RWclose(rwop);
    if(!image)
      throw std::runtime_error(std::string("can't open ").append(filename));
  }

  SDL_Color pixel_color(int x, int y) const
  {
    size_t w = width();
    size_t h = height();

    if((x < 0) || (static_cast<size_t>(x) >= w) ||
       (y < 0) || (static_cast<size_t>(y) >= h))
    {
      throw std::out_of_range("searching for a pixel out of surface");
    }

    y = static_cast<size_t>(h) - y;
    int bpp = image->format->BytesPerPixel;
    Uint8 *ptr = static_cast<Uint8*>(image->pixels) + y * image->pitch + x * bpp;

    Uint32 p;
    switch (bpp)
    {
      case 1:
        p = *ptr;
        break;

      case 2:
        p = *reinterpret_cast<Uint16*>(ptr);
        break;

      case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
          p = (ptr[0] << 16 | ptr[1] << 8 | ptr[2]);
        else
          p = (ptr[0] | ptr[1] << 8 | ptr[2] << 16);
        break;

      case 4:
        p = *reinterpret_cast<Uint32*>(ptr);
        break;

      default:
        throw std::runtime_error("unknown pixel format : strange bpp");
    }

    SDL_Color rgb;
    SDL_GetRGB(p, image->format, &rgb.r, &rgb.g, &rgb.b);
    return rgb;
  }
  
  
    class no_file : public std::exception
    {
        public:
            virtual const char* what() const noexcept override
            { return "No such file"; }
    };
};






class obj_model
{
private:
  std::vector<vec3d> vertices;
  std::vector<std::vector<vec3i>> faces;

  double max_x;
  double min_x;
  double max_y;
  double min_y;
  double max_z;
  double min_z;

  std::vector<vec2d> texture_verts;
  tga_image diffuse;

public:
  obj_model(std::filesystem::path file_path)
  {
    if(file_path.has_filename() == false)
      throw std::runtime_error("Empty filename");

    if(file_path.extension() != ".obj")
      throw std::runtime_error("Not .obj file");
    
    std::ifstream ifs;
    ifs.open(file_path, std::ifstream::in);

    if(ifs.fail())
      throw std::ios_base::failure("Can't open model obj file");

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
        if(v[2] > max_z)
          max_z = v[2];
        if(v[2] < min_z)
          min_z = v[2];
        vertices.push_back(v);
      }
      else if(str == "vt")
      {
        vec2d tmp;
        ifs >> tmp[0] >> tmp[1];
        texture_verts.push_back(tmp);
        //ifs >> tmp[0];
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
          char *end1 = nullptr;
          char *end2 = nullptr;
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

    try
    {
      diffuse.read_tga(file_path.replace_extension("_diffuse.tga"));
    }
    catch(tga_image::no_file& e)
    {
      std::cout << "Warning: no texture found"  << std::endl;
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
  double zshift() const { return (max_z + min_z) / 2; }


  double xborder() const { return (max_x - min_x) / 2; }
  double yborder() const { return (max_y - min_y) / 2; }

  double max() const
  {
    double x = std::max(max_x, -min_x);
    double y = std::max(max_y, -min_y);
    return std::max(x, y);
  }

  vec3d vertice(size_t i) const { return vertices[i]; }

  std::vector<int> face(size_t i)
  {
    std::vector<int> res;
    for(auto& elem : faces[i])
      res.push_back(elem[0]);
    return res;
  }

  vec2i tv(size_t nface, size_t nvert)
  {
    size_t i = faces[nface][nvert][1];
    return vec2i(texture_verts[i].x * diffuse.width(),
                 texture_verts[i].y * diffuse.height());
  }

  SDL_Color tv_clr(size_t nface, size_t nvert)
  {
    vec2i t = tv(nface, nvert);
    return diffuse.pixel_color(t.x, t.y);
  }

  SDL_Color tv_clr(int x, int y)
  {
    return diffuse.pixel_color(x, y);
  }
};

