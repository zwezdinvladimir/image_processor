#pragma once
#include "image.h"

class Filter {
  public:
    virtual ~Filter() = default;
    virtual void apply(Image &image) const = 0;
};
