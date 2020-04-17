#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "geometry.hpp"

class obj_model
{
private:
  std::vector<vec3d> vertices;
  std::vector<std::vector<int>> faces;
public:
  obj_model(const char *filename)
  {
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::in);
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
        vertices.push_back(v);
      }
      else if(str == "f")
      {
        std::vector<int> f;
        int tmp;

        for(size_t i = 0; i < 3; ++i)
        {
          ifs >> tmp;
          f.push_back(tmp - 1);
          std::string tmpstr;
          ifs >> tmpstr;
        }

        faces.push_back(f);
      }
    }
  }

  size_t nvertices() const { return vertices.size(); }

  size_t nfaces() const { return faces.size(); }

  vec3d vertice(size_t i) const { return vertices[i]; }

  std::vector<int> face(size_t i) const { return faces[i]; }
};

