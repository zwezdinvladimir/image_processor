#pragma once
#include "filter_sequence.h"
struct EdgeDetection : public FilterSequence {
  public:
    EdgeDetection(double);

  private:
    using FilterSequence::add;
};