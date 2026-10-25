#include "sharpening.h"

Sharpening::Sharpening() : Convolution({{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}) {}