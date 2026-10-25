#pragma once

#include "filter.h"

struct GaussianBlur : public Filter {
  public:
    GaussianBlur(double);

    void apply(Image &) const override;

  private:
    double sigma_;
};
