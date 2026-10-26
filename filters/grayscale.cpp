#include "grayscale.h"
#include <cmath>
#include <cstddef>
#include <stdexcept>

Grayscale::Grayscale(double kr, double kg, double kb) {
    if (!std::isfinite(kr) || !std::isfinite(kg) || !std::isfinite(kb) || kr < 0 || kg < 0 || kb < 0) {
        throw std::invalid_argument("Grayscale coefficients must be non-negative and finite");
    }
    double s = kr + kg + kb;
    if (!std::isfinite(s) || s <= 0) {
        throw std::invalid_argument("Grayscale sum of coefficients must be positive and finite");
    }
    kr_ = kr / s;
    kg_ = kg / s;
    kb_ = kb / s;
}
void Grayscale::apply(Image &image) const {
    for (std::size_t i = 0; i < image.get_width(); ++i) {
        for (std::size_t j = 0; j < image.get_height(); ++j) {
            double gray = kr_ * image.at(i, j).get_red() + kg_ * image.at(i, j).get_green() +
                          kb_ * image.at(i, j).get_blue();
            image.at(i, j) = Color(gray, gray, gray);
        }
    }
}