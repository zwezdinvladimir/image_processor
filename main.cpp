#include "filters/convolution.h"
#include "filters/crop.h"
#include "filters/edge_detection.h"
#include "filters/filter_sequence.h"
#include "filters/gaussian_blur.h"
#include "filters/grayscale.h"
#include "filters/negative.h"
#include "filters/quadtree_mosaic.h"
#include "filters/sharpening.h"
#include "filters/threshold.h"
#include "image.h"
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

std::size_t parse_size_t(const std::string &s) {
    std::size_t value = 0;
    auto result = std::from_chars(s.data(), s.data() + s.size(), value);
    if (result.ec != std::errc{} || result.ptr != s.data() + s.size() || value == 0) {
        throw std::invalid_argument("Expected a positive integer size");
    }
    return value;
}
double parse_double(const std::string &s) {
    if (s.empty() || s.find_first_of("xX") != std::string::npos ||
        std::isspace(static_cast<unsigned char>(s.front()))) {
        throw std::invalid_argument("Invalid argument");
    }
    std::size_t used;
    double x = std::stod(s, &used);
    if (used < s.size() || !std::isfinite(x)) {
        throw std::invalid_argument("Invalid argument");
    }
    return x;
}

int main(int argc, char *argv[]) {
    try {
        if (argc == 1) {
            std::cout << "Image_processor path_to_input.bmp path_to_output.bmp [filter1] [filter2] ...\n";
            std::cout << "[-crop width height] - crop image\n";
            std::cout << "[-gs] - convert image to shades of gray\n";
            std::cout << "[-neg] - convert image to negative\n";
            std::cout << "[-sharp] - sharpen image\n";
            std::cout << "[-edge threshold] - edge detection with threshold\n";
            std::cout << "[-blur sigma] - apply gaussian blur with standard deviation equal to sigma\n";
            std::cout << "[-quad threshold] - apply quadtree mosaic transform\n";
            return 0;
        }
        if (argc < 3) {
            throw std::invalid_argument("Expected input and output files");
        }
        Image image = read_bmp(argv[1]);

        for (int i = 3; i < argc; ++i) {
            std::string op = argv[i];
            if (op == "-crop") {
                if (argc - i < 3) {
                    throw std::invalid_argument("-crop needs width and height");
                }
                std::size_t width = parse_size_t(argv[i + 1]);
                std::size_t height = parse_size_t(argv[i + 2]);
                Crop filter(width, height);
                filter.apply(image);
                i += 2;
            } else if (op == "-gs") {
                Grayscale filter(0.299, 0.587, 0.114);
                filter.apply(image);
            } else if (op == "-neg") {
                Negative filter;
                filter.apply(image);
            } else if (op == "-sharp") {
                Sharpening filter;
                filter.apply(image);
            } else if (op == "-edge") {
                if (argc - i < 2) {
                    throw std::invalid_argument("-edge needs threshold");
                }
                double threshold = parse_double(argv[i + 1]);
                EdgeDetection filter(threshold);
                filter.apply(image);
                i += 1;
            } else if (op == "-blur") {
                if (argc - i < 2) {
                    throw std::invalid_argument("-blur needs sigma");
                }
                double sigma = parse_double(argv[i + 1]);
                GaussianBlur filter(sigma);
                filter.apply(image);
                i += 1;
            } else if (op == "-quad") {
                if (argc - i < 2) {
                    throw std::invalid_argument("-quad needs threshold");
                }
                double threshold = parse_double(argv[i + 1]);
                QuadtreeMosaic filter(threshold);
                filter.apply(image);
                i += 1;
            } else {
                throw std::invalid_argument("Unknown option");
            }
        }

        image.write_bmp(argv[2]);
    } catch (const std::exception &error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
}