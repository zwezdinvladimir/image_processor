#include "quadtree_mosaic.h"
#include <cmath>
#include <cstddef>
#include <stdexcept>

QuadtreeMosaic::QuadtreeMosaic(double threshold) {
    if (!std::isfinite(threshold) || threshold < 0) {
        throw std::invalid_argument("Threshold for quadtree mosaic must be non-negative and finite");
    }
    threshold_ = threshold;
}
void QuadtreeMosaic::apply(Image &image) const {
    auto paint = [this, &image](this auto &&paint, std::size_t l_i, std::size_t r_i, std::size_t l_j,
                                std::size_t r_j) -> void {
        std::size_t size = (r_i - l_i) * (r_j - l_j);
        if (size <= 1) {
            return;
        }
        Color mean;
        for (std::size_t i = l_i; i < r_i; ++i) {
            for (std::size_t j = l_j; j < r_j; ++j) {
                mean += image.at(i, j);
            }
        }
        mean /= size;
        Color sigma;
        for (std::size_t i = l_i; i < r_i; ++i) {
            for (std::size_t j = l_j; j < r_j; ++j) {
                sigma += (mean - image.at(i, j)) * (mean - image.at(i, j));
            }
        }
        sigma /= size;
        if ((sigma.get_red() + sigma.get_green() + sigma.get_blue()) / 3 <= threshold_) {
            for (std::size_t i = l_i; i < r_i; ++i) {
                for (std::size_t j = l_j; j < r_j; ++j) {
                    image.at(i, j) = mean;
                }
            }
        } else {
            std::size_t mid_i = l_i + (r_i - l_i) / 2;
            std::size_t mid_j = l_j + (r_j - l_j) / 2;
            paint(l_i, mid_i, l_j, mid_j);
            paint(mid_i, r_i, l_j, mid_j);
            paint(l_i, mid_i, mid_j, r_j);
            paint(mid_i, r_i, mid_j, r_j);
        }
    };
    paint(0, image.get_width(), 0, image.get_height());
}