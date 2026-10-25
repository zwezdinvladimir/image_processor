#pragma once
#include "filter.h"

struct Threshold : public Filter {
  public:
    Threshold(double);
    Threshold(double, double, double);

    void apply(Image &) const override;

  private:
    double threshold_r_;
    double threshold_g_;
    double threshold_b_;
};
