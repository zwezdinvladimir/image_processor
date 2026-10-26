#include "gaussian_blur.h"
#include <algorithm>
#include <cmath>
#include <cstddef>

GaussianBlur::GaussianBlur(double sigma) {
    if (!std::isfinite(sigma) || sigma <= 0) {
        throw std::invalid_argument("Sigma must be finite and positive");
    }
    double scale = 2.0 * M_PI * sigma * sigma;
    if (!std::isfinite(scale) || !std::isfinite(1 / scale)) {
        throw std::invalid_argument("Sigma is too small or too large");
    }
    sigma_ = sigma;
}

void GaussianBlur::apply(Image &image) const {
    std::size_t width = image.get_width();
    std::size_t height = image.get_height();
    Image vertical(width, height);
    Image new_image(width, height);
    std::size_t lim = std::max(width, height);
    double required = std::max(5.0, std::ceil(3 * sigma_));
    std::size_t radius = required <= lim ? required : lim;
    for (std::size_t i = 0; i < width; ++i) {
        for (std::size_t j = 0; j < height; ++j) {
            for (std::size_t i1 = i >= radius ? i - radius : 0; i1 <= i + radius && i1 < width; ++i1) {
                double dist_i = static_cast<double>(i >= i1 ? i - i1 : i1 - i);
                vertical.at(i, j) += image.at(i1, j) * std::exp(-dist_i * dist_i / (2 * sigma_ * sigma_));
            }
        }
    }
    for (std::size_t i = 0; i < width; ++i) {
        for (std::size_t j = 0; j < height; ++j) {
            for (std::size_t j1 = j >= radius ? j - radius : 0; j1 <= j + radius && j1 < height; ++j1) {
                double dist_j = static_cast<double>(j >= j1 ? j - j1 : j1 - j);
                new_image.at(i, j) += vertical.at(i, j1) * std::exp(-dist_j * dist_j / (2 * sigma_ * sigma_));
            }
        }
    }
    for (std::size_t i = 0; i < width; ++i) {
        for (std::size_t j = 0; j < height; ++j) {
            new_image.at(i, j) /= 2 * M_PI * sigma_ * sigma_;
            new_image.at(i, j).clamp();
        }
    }
    image = std::move(new_image);
}