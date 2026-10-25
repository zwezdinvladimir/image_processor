#include "filters/crop.h"
#include "filters/edge_detection.h"
#include "filters/gaussian_blur.h"
#include "filters/grayscale.h"
#include "filters/negative.h"
#include "filters/quadtree_mosaic.h"
#include "filters/sharpening.h"
#include "filters/threshold.h"
#include "image.h"
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
int failures = 0;

void check(bool condition, const std::string &name) {
    if (!condition) {
        ++failures;
        std::cerr << "FAIL: " << name << '\n';
    }
}

template <typename F> bool throws(F function) {
    try {
        function();
    } catch (const std::exception &) {
        return true;
    }
    return false;
}

bool near(double a, double b) {
    return std::abs(a - b) < 1.5 / 255;
}

bool same_color(const Color &a, const Color &b) {
    return near(a.get_red(), b.get_red()) && near(a.get_green(), b.get_green()) &&
           near(a.get_blue(), b.get_blue());
}

std::filesystem::path test_directory() {
    static const auto path = std::filesystem::temp_directory_path() /
                             ("image_processor_tests_" +
                              std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(path);
    return path;
}

std::filesystem::path save_bytes(const std::string &name, const std::vector<unsigned char> &bytes) {
    auto path = test_directory() / name;
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    return path;
}

// Make a small 24-bit BMP with simple, predictable pixels.
std::vector<unsigned char> make_bmp(int width, int height, int offset) {
    int rows = height < 0 ? -height : height;
    int row_size = (width * 3 + 3) / 4 * 4;
    int file_size = offset + row_size * rows;
    std::vector<unsigned char> bytes(file_size, 0);

    auto put = [&bytes](int pos, std::uint32_t value, int count) {
        for (int i = 0; i < count; ++i) {
            bytes[pos + i] = static_cast<unsigned char>(value >> (8 * i));
        }
    };

    bytes[0] = 'B';
    bytes[1] = 'M';
    put(2, file_size, 4);
    put(10, offset, 4);
    put(14, 40, 4);
    put(18, width, 4);
    put(22, static_cast<std::uint32_t>(height), 4);
    put(26, 1, 2);
    put(28, 24, 2);
    put(34, row_size * rows, 4);

    for (int y = 0; y < rows; ++y) {
        int file_y = height > 0 ? rows - 1 - y : y;
        for (int x = 0; x < width; ++x) {
            int pixel = offset + file_y * row_size + x * 3;
            bytes[pixel] = 100;             // blue
            bytes[pixel + 1] = y * 30 + 10; // green
            bytes[pixel + 2] = x * 30 + 20; // red
        }
    }
    return bytes;
}

void test_filters() {
    Image image(1, 1);
    image.at(0, 0) = Color(0.2, 0.4, 0.6);
    Negative().apply(image);
    check(same_color(image.at(0, 0), Color(0.8, 0.6, 0.4)), "negative");

    image.at(0, 0) = Color(1, 0, 0);
    Grayscale(0.299, 0.587, 0.114).apply(image);
    check(same_color(image.at(0, 0), Color(0.299, 0.299, 0.299)), "grayscale");

    image.at(0, 0) = Color(0.5, 0.7, 0.2);
    Threshold(0.5).apply(image);
    check(same_color(image.at(0, 0), Color(0, 1, 0)), "threshold uses strict comparison");

    Image crop(2, 2);
    crop.at(0, 0) = Color(0.1, 0.2, 0.3);
    crop.at(0, 1) = Color(0.7, 0.8, 0.9);
    Crop(1, 2).apply(crop);
    check(crop.get_width() == 1 && crop.get_height() == 2 && same_color(crop.at(0, 1), Color(0.7, 0.8, 0.9)),
          "crop keeps top-left part");

    check(throws([] { Threshold(-1); }), "negative threshold is rejected");
    check(throws([] { QuadtreeMosaic(-1); }), "negative mosaic threshold is rejected");
    check(throws([] { GaussianBlur(0); }), "zero blur sigma is rejected");
}

void test_filters_on_simple_images() {
    Image uniform(3, 3);
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            uniform.at(x, y) = Color(0.5, 0.5, 0.5);
        }
    }
    EdgeDetection(0.1).apply(uniform);
    check(same_color(uniform.at(1, 1), Color(0, 0, 0)), "edge detection on uniform image");

    Image sharp(3, 3);
    sharp.at(1, 1) = Color(1, 1, 1);
    Sharpening().apply(sharp);
    check(same_color(sharp.at(1, 1), Color(1, 1, 1)), "sharpening");

    Image mosaic(2, 2);
    for (int y = 0; y < 2; ++y) {
        for (int x = 0; x < 2; ++x) {
            mosaic.at(x, y) = Color(0.25, 0.5, 0.75);
        }
    }
    QuadtreeMosaic(0).apply(mosaic);
    check(same_color(mosaic.at(0, 0), Color(0.25, 0.5, 0.75)), "mosaic keeps uniform image");

    GaussianBlur(1).apply(mosaic);
    check(mosaic.get_width() == 2 && mosaic.get_height() == 2, "blur keeps image dimensions");
}

void test_bmp_padding() {
    // Widths 1-4 cover every possible row-padding length.
    for (int width = 1; width <= 4; ++width) {
        auto path = save_bytes("padding_" + std::to_string(width) + ".bmp", make_bmp(width, 2, 54));
        Image image = read_bmp(path);
        check(image.get_width() == static_cast<std::size_t>(width) && image.get_height() == 2,
              "BMP dimensions and padding");
        check(near(image.at(0, 0).get_red(), 20.0 / 255) && near(image.at(0, 1).get_green(), 40.0 / 255),
              "BMP pixels read in correct order");
    }
}

void test_bmp_layout_and_errors() {
    // Negative height means top-down rows; offset 70 leaves extra header bytes.
    auto path = save_bytes("top_down.bmp", make_bmp(2, -2, 70));
    Image top_down = read_bmp(path);
    check(near(top_down.at(0, 0).get_green(), 10.0 / 255), "BMP top-down rows and larger offset");

    auto truncated = make_bmp(2, 2, 54);
    truncated.resize(60);
    path = save_bytes("truncated.bmp", truncated);
    check(throws([&] { read_bmp(path); }), "truncated BMP is rejected");
}

void test_bmp_round_trip() {
    Image original(2, 2);
    original.at(0, 0) = Color(1, 0, 0);
    original.at(1, 0) = Color(0, 1, 0);
    original.at(0, 1) = Color(0, 0, 1);
    original.at(1, 1) = Color(0.2, 0.4, 0.6);

    auto path = test_directory() / "round_trip.bmp";
    original.write_bmp(path);
    Image loaded = read_bmp(path);
    check(loaded.get_width() == 2 && loaded.get_height() == 2, "BMP round-trip dimensions");
    check(same_color(loaded.at(1, 1), original.at(1, 1)), "BMP round-trip pixel values");
}
} // namespace

int main(int argc, char *argv[]) {
    std::string group = argc > 1 ? argv[1] : "all";
    if (group == "all" || group == "filters") {
        test_filters();
    }
    if (group == "all" || group == "effects") {
        test_filters_on_simple_images();
    }
    if (group == "all" || group == "bmp_padding") {
        test_bmp_padding();
    }
    if (group == "all" || group == "bmp_layout") {
        test_bmp_layout_and_errors();
    }
    if (group == "all" || group == "bmp_round_trip") {
        test_bmp_round_trip();
    }
    if (group != "all" && group != "filters" && group != "effects" && group != "bmp_padding" &&
        group != "bmp_layout" && group != "bmp_round_trip") {
        std::cerr << "Unknown test group: " << group << '\n';
        return 2;
    }

    std::error_code error;
    std::filesystem::remove_all(test_directory(), error);

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }
    std::cout << "All tests passed\n";
    return 0;
}
