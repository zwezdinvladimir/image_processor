#include "negative.h"

void Negative::apply(Image &image) const {
    for (size_t i = 0; i < image.get_width(); ++i) {
        for (size_t j = 0; j < image.get_height(); ++j) {
            image.at(i, j) = Color(1, 1, 1) - image.at(i, j);
        }
    }
}