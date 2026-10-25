#pragma once
#include "filter.h"

struct Negative : public Filter {
  public:
    void apply(Image &) const override;
};
