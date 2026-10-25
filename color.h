#pragma once
#include <fstream>

struct Color {
  public:
    Color();
    Color(double, double, double);

    void set_red(double);
    void set_blue(double);
    void set_green(double);
    double get_red() const;
    double get_blue() const;
    double get_green() const;
    void clamp();
    Color &operator+=(const Color &);
    Color &operator-=(const Color &);
    Color &operator*=(const Color &);
    Color &operator/=(const Color &);
    Color &operator+=(double);
    Color &operator-=(double);
    Color &operator*=(double);
    Color &operator/=(double);

    void write_bmp(std::ofstream &) const;

  private:
    double r_ = 0, g_ = 0, b_ = 0;
};
Color operator+(Color, const Color &);
Color operator-(Color, const Color &);
Color operator*(Color, const Color &);
Color operator/(Color, const Color &);
Color operator+(Color, double);
Color operator-(Color, double);
Color operator*(Color, double);
Color operator/(Color, double);
Color read_bmp(std::ifstream &);