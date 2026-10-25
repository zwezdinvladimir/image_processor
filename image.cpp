#include "image.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>

Image::Image() {
    width_ = 0;
    height_ = 0;
}
Image::Image(std::size_t width, std::size_t height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Image dimensions must be positive");
    }
    if (width > pixels_.max_size() / height) {
        throw std::length_error("Image dimensions are too large");
    }
    width_ = width;
    height_ = height;
    pixels_.assign(width_ * height_, Color());
}
std::size_t Image::get_width() const {
    return width_;
}
std::size_t Image::get_height() const {
    return height_;
}
Color &Image::at(std::size_t x, std::size_t y) {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("x or y is out of image");
    }
    return pixels_.at(x + y * width_);
}
const Color &Image::at(std::size_t x, std::size_t y) const {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("x or y is out of image");
    }
    return pixels_.at(x + y * width_);
}

namespace {
constexpr std::size_t kHeaderSize = 54;
constexpr std::uint32_t kInfoHeaderSize = 40;
using BmpHeader = std::array<unsigned char, kHeaderSize>;

std::uint32_t read_field(const BmpHeader &header, std::size_t offset, std::size_t size) {
    std::uint32_t value = 0;
    for (std::size_t i = 0; i < size; ++i) {
        value |= static_cast<std::uint32_t>(header[offset + i]) << (8 * i);
    }
    return value;
}

std::int64_t read_dimension(const BmpHeader &header, std::size_t offset) {
    std::uint32_t value = read_field(header, offset, 4);
    return value <= 0x7FFFFFFFu ? value : static_cast<std::int64_t>(value) - 0x100000000LL;
}

void write_field(BmpHeader &header, std::size_t offset, std::uint32_t value, std::size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
        header[offset + i] = static_cast<unsigned char>((value >> (8 * i)) & 0xFFu);
    }
}
} // namespace
Image read_bmp(const std::filesystem::path &input_file) {
    std::ifstream input(input_file, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Failed to open BMP file: " + input_file.string());
    }

    BmpHeader header{};
    if (!input.read(reinterpret_cast<char *>(header.data()), kHeaderSize)) {
        throw std::runtime_error("Failed to read BMP headers");
    }
    if (header[0] != 'B' || header[1] != 'M') {
        throw std::runtime_error("File is not BMP");
    }
    if (read_field(header, 14, 4) != kInfoHeaderSize || read_field(header, 26, 2) != 1 ||
        read_field(header, 28, 2) != 24 || read_field(header, 30, 4) != 0 || read_field(header, 46, 4) != 0) {
        throw std::runtime_error("Expected uncompressed 24-bit BMP with BITMAPINFOHEADER and no color table");
    }
    std::int64_t signed_width = read_dimension(header, 18);
    std::int64_t signed_height = read_dimension(header, 22);
    std::uint32_t pixel_offset = read_field(header, 10, 4);
    if (signed_width <= 0 || signed_height == 0 || pixel_offset < kHeaderSize) {
        throw std::runtime_error("Invalid BMP dimensions or pixel offset");
    }
    bool bottom_up = signed_height > 0;
    std::uint64_t width = static_cast<std::uint64_t>(signed_width);
    std::uint64_t height = static_cast<std::uint64_t>(bottom_up ? signed_height : -signed_height);
    std::uint64_t row_bytes = width * 3;
    std::uint64_t padding = (4 - row_bytes % 4) % 4;
    std::uint64_t pixel_bytes = (row_bytes + padding) * height;
    std::uint64_t required_size = pixel_offset + pixel_bytes;

    input.seekg(0, std::ios::end);
    std::streamoff file_size = input.tellg();
    if (!input || file_size < 0) {
        throw std::runtime_error("Failed to determine BMP file size");
    }
    if (required_size > static_cast<std::uint64_t>(file_size)) {
        throw std::runtime_error("Truncated BMP data");
    }
    if (width > std::numeric_limits<std::size_t>::max() / height) {
        throw std::length_error("BMP dimensions are too large");
    }

    Image image(static_cast<std::size_t>(width), static_cast<std::size_t>(height));
    input.seekg(pixel_offset, std::ios::beg);
    if (!input) {
        throw std::runtime_error("Failed to seek to BMP pixels");
    }
    for (std::size_t row = 0; row < image.get_height(); ++row) {
        std::size_t y = bottom_up ? image.get_height() - 1 - row : row;
        for (std::size_t x = 0; x < image.get_width(); ++x) {
            image.at(x, y) = read_bmp(input);
        }
        char extra[3];
        if (!input.read(extra, static_cast<std::streamsize>(padding))) {
            throw std::runtime_error("Failed to read BMP row padding");
        }
    }
    return image;
}

void Image::write_bmp(const std::filesystem::path &output_file) const {
    if (width_ == 0 || height_ == 0 ||
        width_ > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()) ||
        height_ > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())) {
        throw std::runtime_error("Image dimensions cannot be stored in BMP");
    }
    std::uint64_t row_bytes = static_cast<std::uint64_t>(width_) * 3;
    std::uint64_t padding = (4 - row_bytes % 4) % 4;
    std::uint64_t pixel_bytes = (row_bytes + padding) * height_;
    std::uint64_t file_size = kHeaderSize + pixel_bytes;
    if (file_size > std::numeric_limits<std::uint32_t>::max()) {
        throw std::length_error("Image is too large for BMP");
    }

    BmpHeader header{};
    header[0] = 'B';
    header[1] = 'M';
    write_field(header, 2, static_cast<std::uint32_t>(file_size), 4);
    write_field(header, 10, kHeaderSize, 4);
    write_field(header, 14, kInfoHeaderSize, 4);
    write_field(header, 18, static_cast<std::uint32_t>(width_), 4);
    write_field(header, 22, static_cast<std::uint32_t>(height_), 4);
    write_field(header, 26, 1, 2);
    write_field(header, 28, 24, 2);
    write_field(header, 34, static_cast<std::uint32_t>(pixel_bytes), 4);

    std::ofstream output(output_file, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Failed to open output BMP file: " + output_file.string());
    }
    output.write(reinterpret_cast<const char *>(header.data()), kHeaderSize);
    const char extra[3] = {};
    for (std::size_t row = height_; row > 0; --row) {
        for (std::size_t x = 0; x < width_; ++x) {
            at(x, row - 1).write_bmp(output);
        }
        output.write(extra, static_cast<std::streamsize>(padding));
    }
    output.close();
    if (!output) {
        throw std::runtime_error("Failed to write BMP file: " + output_file.string());
    }
}
