#include "crop.h"
#include <algorithm>
#include <cstddef>
#include <utility>

Crop::Crop(std::size_t width, std::size_t height) {
    width_ = width;
    height_ = height;
}
void Crop::apply(Image &image) const {
    std::size_t width = std::min(image.get_width(), width_);
    std::size_t height = std::min(image.get_height(), height_);
    Image new_image(width, height);
    for (std::size_t i = 0; i < width; ++i) {
        for (std::size_t j = 0; j < height; ++j) {
            new_image.at(i, j) = image.at(i, j);
        }
    }
    image = std::move(new_image);
}