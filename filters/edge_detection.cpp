#include "edge_detection.h"
#include "convolution.h"
#include "grayscale.h"
#include "threshold.h"

EdgeDetection::EdgeDetection(double threshold)
    : FilterSequence(Grayscale(0.299, 0.587, 0.114), Convolution({{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}}),
                     Threshold(threshold)) {}