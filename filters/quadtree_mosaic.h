#pragma once
#include "filter.h"

struct QuadtreeMosaic : public Filter {
  public:
    QuadtreeMosaic(double);

    void apply(Image &) const override;

  private:
    double threshold_;
};