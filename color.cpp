#include "color.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>

Color::Color() {
    r_ = g_ = b_ = 0;
}
Color::Color(double r, double g, double b) {
    set_red(r);
    set_green(g);
    set_blue(b);
}
void Color::clamp() {
    r_ = std::clamp<double>(r_, 0, 1);
    g_ = std::clamp<double>(g_, 0, 1);
    b_ = std::clamp<double>(b_, 0, 1);
}
void Color::set_red(double r) {
    r_ = r;
}
void Color::set_green(double g) {
    g_ = g;
}
void Color::set_blue(double b) {
    b_ = b;
}
double Color::get_red() const {
    return r_;
}
double Color::get_green() const {
    return g_;
}
double Color::get_blue() const {
    return b_;
}
Color &Color::operator+=(const Color &other) {
    r_ += other.r_;
    g_ += other.g_;
    b_ += other.b_;
    return *this;
}
Color operator+(Color lhs, const Color &rhs) {
    return lhs += rhs;
}
Color &Color::operator-=(const Color &other) {
    r_ -= other.r_;
    g_ -= other.g_;
    b_ -= other.b_;
    return *this;
}
Color operator-(Color lhs, const Color &rhs) {
    return lhs -= rhs;
}
Color &Color::operator*=(const Color &other) {
    r_ *= other.r_;
    g_ *= other.g_;
    b_ *= other.b_;
    return *this;
}
Color operator*(Color lhs, const Color &rhs) {
    return lhs *= rhs;
}
Color &Color::operator/=(const Color &other) {
    r_ /= other.r_;
    g_ /= other.g_;
    b_ /= other.b_;
    return *this;
}
Color operator/(Color lhs, const Color &rhs) {
    return lhs /= rhs;
}
Color &Color::operator+=(double other) {
    r_ += other;
    g_ += other;
    b_ += other;
    return *this;
}
Color operator+(Color lhs, double rhs) {
    return lhs += rhs;
}
Color &Color::operator-=(double other) {
    r_ -= other;
    g_ -= other;
    b_ -= other;
    return *this;
}
Color operator-(Color lhs, double rhs) {
    return lhs -= rhs;
}
Color &Color::operator*=(double other) {
    r_ *= other;
    g_ *= other;
    b_ *= other;
    return *this;
}
Color operator*(Color lhs, double rhs) {
    return lhs *= rhs;
}
Color &Color::operator/=(double other) {
    r_ /= other;
    g_ /= other;
    b_ /= other;
    return *this;
}
Color operator/(Color lhs, double rhs) {
    return lhs /= rhs;
}
Color read_bmp(std::ifstream &input) {
    std::uint8_t raw[3];
    if (!input.read(reinterpret_cast<char *>(raw), 3)) {
        throw std::runtime_error("Failed to read pixel");
    }
    return Color(static_cast<double>(raw[2]) / 255, static_cast<double>(raw[1]) / 255,
                 static_cast<double>(raw[0]) / 255);
}
void Color::write_bmp(std::ofstream &output) const {
    if (!std::isfinite(r_) || !std::isfinite(g_) || !std::isfinite(b_)) {
        throw std::runtime_error("Cannot write non-finite color");
    }
    auto copy = *this;
    copy.clamp();
    std::uint8_t r = static_cast<std::uint8_t>(std::round(copy.r_ * 255));
    std::uint8_t g = static_cast<std::uint8_t>(std::round(copy.g_ * 255));
    std::uint8_t b = static_cast<std::uint8_t>(std::round(copy.b_ * 255));
    output.put(static_cast<char>(b));
    output.put(static_cast<char>(g));
    output.put(static_cast<char>(r));
    if (output.fail()) {
        throw std::runtime_error("Failed to write pixel");
    }
}