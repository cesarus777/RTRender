#pragma once

#include <array>
#include <cmath>
#include <stdexcept>

template <typename T, size_t size> struct vec {};

using vec2d = vec<double, 2>;
using vec2i = vec<int, 2>;
using vec3d = vec<double, 3>;
using vec3i = vec<int, 3>;

template <typename T> struct vec<T, 3>
{
  T x, y, z;
  vec() : x(0), y(0), z(0) {}
  vec(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
  template <typename U> vec(const vec<U, 3> &v) : x(v.x), y(v.y), z(v.z) {}

  T& operator[](const size_t i)
  {
    switch(i)
    {
      case 0:
        return x;
      case 1:
        return y;
      case 2:
        return z;
      default:
        throw std::out_of_range("vec3.operator[]");
    }
  }

  vec<T, 3> operator^(const vec<T, 3> &another) const
  {
    return vec<T, 3>(y * another.z - z * another.y,
                   z * another.x - x * another.z,
                   x * another.y - y * another.x);
  }

  vec<T, 3> operator+(const vec<T, 3> &another) const
  {
    return vec<T, 3>(x + another.x,
                   y + another.y,
                   z + another.z);
  }

  vec<T, 3> operator-(const vec<T, 3> &another) const
  {
    return vec<T, 3>(x - another.x,
                   y - another.y,
                   z - another.z);
  }

  vec<T, 3> operator*(const double k) const
  {
    return vec<T, 3>(x * k, y * k, z * k);
  }

  double operator*(const vec<T, 3> &another) const
  {
    return x * another.x + y * another.y + z * another.z;
  }

  double norm() const
  {
    return std::sqrt(x * x + y * y + z * z);
  }

  vec<T, 3>& normalize()
  {
    *this = *this * (1.0 / norm());
    return *this;
  }
};

template <typename T> struct vec<T, 2>
{
  T x, y;
  vec() : x(0), y(0) {}
  vec(T _x, T _y) : x(_x), y(_y) {}
  template <typename U> vec(const vec<U, 2> &v) : x(v.x), y(v.y) {}

  T& operator[](const size_t i)
  {
    switch(i)
    {
      case 0:
        return x;
      case 1:
        return y;
      default:
        throw std::out_of_range("vec2.operator[]");
    }
  }

  vec<T, 2> operator+(const vec<T, 2> &another) const
  {
    return vec<T, 2>(x + another.x,
                   y + another.y);
  }

  vec<T, 2> operator-(const vec<T, 2> &another) const
  {
    return vec<T, 2>(x - another.x,
                   y - another.y);
  }

  vec<T, 2> operator*(const double k) const
  {
    return vec<T, 2>(x * k, y * k);
  }

  double operator*(const vec<T, 2> &another) const
  {
    return x * another.x + y * another.y;
  }
};

template <typename T, size_t size> struct triangle
{
  std::array<vec<T, size>, 3> verts;

  triangle() {}

  triangle(vec<T, size> v1, vec<T, size> v2, vec<T, size> v3)
  {
    verts = { v1, v2, v3 };
  }

  auto& operator[](const size_t i)
  {
    return verts[i];
  }
};

