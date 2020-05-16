#pragma once

#include <cmath>
#include <stdexcept>

template <typename T> struct vec3
{
  T x, y, z;
  vec3() : x(0), y(0), z(0) {}
  vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
  template <typename U> vec3(const vec3<U> &v) : x(v.x), y(v.y), z(v.z) {}

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

  vec3<T> operator^(const vec3<T> &another) const
  {
    return vec3<T>(y * another.z - z * another.y,
                   z * another.x - x * another.z,
                   x * another.y - y * another.x);
  }

  vec3<T> operator+(const vec3<T> &another) const
  {
    return vec3<T>(x + another.x,
                   y + another.y,
                   z + another.z);
  }

  vec3<T> operator-(const vec3<T> &another) const
  {
    return vec3<T>(x - another.x,
                   y - another.y,
                   z - another.z);
  }

  vec3<T> operator*(const double k) const
  {
    return vec3<T>(x * k, y * k, z * k);
  }

  double operator*(const vec3<T> &another) const
  {
    return x * another.x + y * another.y + z * another.z;
  }

  double norm() const
  {
    return std::sqrt(x * x + y * y + z * z);
  }

  vec3<T>& normalize()
  {
    *this = *this * (1.0 / norm());
    return *this;
  }
};

template <typename T> struct vec2
{
  T x, y;
  vec2() : x(0), y(0) {}
  vec2(T _x, T _y) : x(_x), y(_y) {}
  template <typename U> vec2(const vec2<U> &v) : x(v.x), y(v.y) {}

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

  vec2<T> operator+(const vec2<T> &another) const
  {
    return vec2<T>(x + another.x,
                   y + another.y);
  }

  vec2<T> operator-(const vec2<T> &another) const
  {
    return vec2<T>(x - another.x,
                   y - another.y);
  }

  vec2<T> operator*(const double k) const
  {
    return vec2<T>(x * k, y * k);
  }

  double operator*(const vec2<T> &another) const
  {
    return x * another.x + y * another.y;
  }
};

typedef vec2<double> vec2d;
typedef vec2<int>    vec2i;
typedef vec3<double> vec3d;
typedef vec3<int>    vec3i;

