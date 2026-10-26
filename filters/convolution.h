#pragma once
#include "filter.h"
#include <cstddef>
#include <vector>

struct Convolution : public Filter {
  public:
    Convolution(std::size_t width = 1, std::size_t height = 1);
    Convolution(std::vector<std::vector<double>>);

    void apply(Image &) const override;

  private:
    std::size_t width_;
    std::size_t height_;
    std::vector<std::vector<double>> kernel_;
};
