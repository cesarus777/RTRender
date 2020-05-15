#pragma once

#include <stdexcept>

class sdl_error : public std::runtime_error
{
public:
  explicit sdl_error(const std::string& what_arg) : std::runtime_error(what_arg) {}
  explicit sdl_error(const char* what_arg) : std::runtime_error(what_arg) {}
};

