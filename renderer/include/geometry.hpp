#pragma once

#include <array>
#include <cmath>
#include <stdexcept>



template <typename T, size_t size>
struct vec
{
  std::array<T, size> coords;

  template <typename ...U>
  constexpr vec(U... _coords) : coords{_coords...} {}

  constexpr T& operator[](const size_t i) { return coords[i]; }
  constexpr const T& operator[](const size_t i) const { return coords[i]; }
};



template <typename T>
struct vec<T, 3>
{
  T x = 0, y = 0, z = 0;

  constexpr vec() {}
  constexpr vec(T _x, T _y, T _z) : x{_x}, y{_y}, z{_z} {}

  template <typename U>
  constexpr vec(const vec<U, 3> &v) : x{static_cast<T>(v.x)},
                                      y{static_cast<T>(v.y)},
                                      z{static_cast<T>(v.z)} {}

  constexpr T& operator[](const size_t i)
  {
    switch(i)
    {
      case 0: return x;
      case 1: return y;
      case 2: return z;

      default: throw std::out_of_range("vec3.operator[]");
    }
  }

  constexpr const T& operator[](const size_t i) const
  {
    switch(i)
    {
      case 0: return x;
      case 1: return y;
      case 2: return z;

      default: throw std::out_of_range("vec3.operator[]");
    }
  }

  constexpr vec<T, 3> operator^(const vec<T, 3> &another) const
  {
    return vec<T, 3>{y * another.z - z * another.y,
                     z * another.x - x * another.z,
                     x * another.y - y * another.x};
  }

  constexpr vec<T, 3> operator+(const vec<T, 3> &another) const
  {
    return vec<T, 3>{x + another.x,
                     y + another.y,
                     z + another.z};
  }

  constexpr vec<T, 3> operator-(const vec<T, 3> &another) const
  {
    return vec<T, 3>{x - another.x,
                     y - another.y,
                     z - another.z};
  }

  constexpr vec<T, 3> operator*(const double k) const
  {
    return vec<T, 3>{x * k, y * k, z * k};
  }

  constexpr double operator*(const vec<T, 3> &another) const
  {
    return x * another.x + y * another.y + z * another.z;
  }

  constexpr double norm() const
  {
    return std::sqrt(x * x + y * y + z * z);
  }

  constexpr vec<T, 3>& normalize()
  {
    *this = *this * (1.0 / norm());
    return *this;
  }
};



template <typename T>
struct vec<T, 2>
{
  T x = 0, y = 0;

  constexpr vec() {}
  constexpr vec(T _x, T _y) : x{_x}, y{_y} {}

  template <typename U>
  constexpr vec(const vec<U, 2> &v) : x{static_cast<T>(v.x)},
                                      y{static_cast<T>(v.y)} {}

  constexpr T& operator[](const size_t i)
  {
    switch(i)
    {
      case 0: return x;
      case 1: return y;

      default: throw std::out_of_range("vec2.operator[]");
    }
  }

  constexpr const T& operator[](const size_t i) const
  {
    switch(i)
    {
      case 0: return x;
      case 1: return y;

      default: throw std::out_of_range("vec2.operator[]");
    }
  }

  constexpr vec<T, 2> operator+(const vec<T, 2> &another) const
  {
    return vec<T, 2>{x + another.x,
                     y + another.y};
  }

  constexpr vec<T, 2> operator-(const vec<T, 2> &another) const
  {
    return vec<T, 2>{x - another.x,
                     y - another.y};
  }

  constexpr vec<T, 2> operator*(const double k) const
  {
    return vec<T, 2>{x * k, y * k};
  }

  constexpr double operator*(const vec<T, 2> &another) const
  {
    return x * another.x + y * another.y;
  }
};



template <typename T>
struct quaternion
{
  T w = 0, x = 0, y = 0, z = 0;

  constexpr quaternion() {}
  constexpr quaternion(T _w, T _x, T _y, T _z) : w{_w}, x{_x}, y{_y}, z{_z} {}

  template <typename U>
  constexpr quaternion(const quaternion<U> &v) : w{v.w}, x{v.x}, y{v.y}, z{v.z} {}

  template <typename U>
  constexpr quaternion(const vec<U, 3> &v) : w{0}, x{v.x}, y{v.y}, z{v.z} {}

  constexpr T& operator[](const size_t i)
  {
    switch(i)
    {
      case 0: return w;
      case 1: return x;
      case 2: return y;
      case 3: return z;

      default: throw std::out_of_range("quaternion<T>.operator[]");
    }
  }

  constexpr const T& operator[](const size_t i) const
  {
    switch(i)
    {
      case 0: return w;
      case 1: return x;
      case 2: return y;
      case 3: return z;

      default: throw std::out_of_range("quaternion<T>.operator[]");
    }
  }

  constexpr quaternion<T> operator*(const quaternion<T> &v) const
  {
    return quaternion<T>{w*v.w - x*v.x - y*v.y - z*v.z,
                         w*v.x + x*v.w + y*v.z - z*v.y,
                         w*v.y + y*v.w - x*v.z + z*v.x,
                         w*v.z + z*v.w + x*v.y - y*v.x};
  }

  template <typename U>
  constexpr void rotate(vec<U, 3>& r) const
  {
    quaternion<U> rq  = r;
    quaternion<U> rev = get_reverse();

    rq = (*this) * rq;
    rq = rq * rev;

    r[0] = rq[1];
    r[1] = rq[2];
    r[2] = rq[3];
  }

  constexpr quaternion<T> get_reverse() const
  {
    double n = norm();
    n *= n;
    return quaternion<T>{w/n, -x/n, -y/n, -z/n};
  }

  constexpr double norm() const
  {

    return std::sqrt(w * w + x * x + y * y + z * z);
  }

  constexpr void normalize()
  {
    double n = norm();
    w /= n;
    x /= n;
    y /= n;
    z /= n;
  }
};



template <typename T, size_t size>
struct triangle
{
  std::array<vec<T, size>, 3> verts;

  constexpr triangle() {}

  constexpr triangle(const vec<T, 3>& v1,
                     const vec<T, 3>& v2,
                     const vec<T, 3>& v3) : verts{ v1, v2, v3 } {}

  constexpr vec<T, 3>& operator[](size_t i)
  {
    return verts[i];
  }

  constexpr const vec<T, 3>& operator[](size_t i) const
  {
    return verts[i];
  }
};



using vec2d = vec<double, 2>;
using vec2i = vec<int, 2>;
using vec3d = vec<double, 3>;
using vec3i = vec<int, 3>;

using triangle2d = triangle<double, 2>;
using triangle2i = triangle<int, 2>;
using triangle3d = triangle<double, 3>;
using triangle3i = triangle<int, 3>;

using quaterniond = quaternion<double>;

using tuple_triangle3d_double_bool = std::tuple<triangle3d, double, bool>;
using tuple_triangle3i_double_bool = std::tuple<triangle3i, double, bool>;

