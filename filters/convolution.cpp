#include "convolution.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

Convolution::Convolution(size_t width, size_t height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Kernel dimensions must be positive");
    }
    if (width > std::numeric_limits<size_t>::max() / 2 || height > std::numeric_limits<size_t>::max() / 2) {
        throw std::length_error("Kernel dimensions are too large");
    }
    width_ = width;
    height_ = height;
    kernel_.assign(2 * height - 1, std::vector<double>(2 * width - 1, 0));
    kernel_[height - 1][width - 1] = 1;
}
Convolution::Convolution(std::vector<std::vector<double>> kernel) {
    if (kernel.size() % 2 == 0 || kernel[0].size() % 2 == 0) {
        throw std::invalid_argument("Both kernel dimensions should be odd");
    }
    for (size_t i = 0; i < kernel.size(); ++i) {
        if (kernel[0].size() != kernel[i].size()) {
            throw std::invalid_argument("Kernel must be rectangle");
        }
        for (size_t j = 0; j < kernel[i].size(); ++j) {
            if (!std::isfinite(kernel[i][j])) {
                throw std::invalid_argument("Kernel coefficients must be finite");
            }
        }
    }
    kernel_ = std::move(kernel);
    width_ = (kernel_[0].size() + 1) / 2;
    height_ = (kernel_.size() + 1) / 2;
}
void Convolution::apply(Image &image) const {
    size_t width = image.get_width();
    size_t height = image.get_height();
    Image new_image(width, height);
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            for (size_t di = 0; di < 2 * width_ - 1; ++di) {
                for (size_t dj = 0; dj < 2 * height_ - 1; ++dj) {
                    size_t ni = i + di;
                    size_t nj = j + dj;
                    ni = ni >= width_ - 1 ? ni - (width_ - 1) : 0;
                    nj = nj >= height_ - 1 ? nj - (height_ - 1) : 0;
                    ni = std::min(ni, width - 1);
                    nj = std::min(nj, height - 1);
                    new_image.at(i, j) += image.at(ni, nj) * kernel_[dj][di];
                }
            }
            new_image.at(i, j).clamp();
        }
    }
    image = std::move(new_image);
}