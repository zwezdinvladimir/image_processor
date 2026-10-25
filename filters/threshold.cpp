#include "threshold.h"
#include <cmath>
#include <stdexcept>

namespace {
void check_threshold(double threshold) {
    if (!std::isfinite(threshold) || threshold < 0) {
        throw std::invalid_argument("Threshold values must be non-negative and finite");
    }
}
} // namespace

Threshold::Threshold(double threshold) {
    check_threshold(threshold);
    threshold_r_ = threshold;
    threshold_g_ = threshold;
    threshold_b_ = threshold;
}
Threshold::Threshold(double threshold_r, double threshold_g, double threshold_b) {
    check_threshold(threshold_r);
    check_threshold(threshold_g);
    check_threshold(threshold_b);
    threshold_r_ = threshold_r;
    threshold_g_ = threshold_g;
    threshold_b_ = threshold_b;
}
void Threshold::apply(Image &image) const {
    for (size_t i = 0; i < image.get_width(); ++i) {
        for (size_t j = 0; j < image.get_height(); ++j) {
            auto c = image.at(i, j);
            image.at(i, j).set_red(c.get_red() > threshold_r_ ? 1 : 0);
            image.at(i, j).set_green(c.get_green() > threshold_g_ ? 1 : 0);
            image.at(i, j).set_blue(c.get_blue() > threshold_b_ ? 1 : 0);
        }
    }
}
