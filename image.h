#pragma once
#include "color.h"
#include <cstddef>
#include <filesystem>
#include <vector>

struct Image {
  public:
    Image();
    Image(std::size_t width, std::size_t height);

    std::size_t get_width() const;
    std::size_t get_height() const;

    Color &at(std::size_t x, std::size_t y);
    const Color &at(std::size_t x, std::size_t y) const;

    void write_bmp(const std::filesystem::path &) const;

  private:
    std::size_t width_, height_;
    std::vector<Color> pixels_;
};
Image read_bmp(const std::filesystem::path &);
