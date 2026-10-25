#pragma once
#include "filter.h"
#include <cstddef>

struct Crop : public Filter {
  public:
    Crop(std::size_t width, std::size_t height);

    void apply(Image &) const override;

  private:
    std::size_t width_, height_;
};
