#pragma once

#include "filter.h"

struct Grayscale : public Filter {
  public:
    Grayscale(double kr = 1, double kg = 1, double kb = 1);

    void apply(Image &) const override;

  private:
    double kr_;
    double kg_;
    double kb_;
};
