#ifndef IMAGE_WRITER_H
#define IMAGE_WRITER_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

#include "color.h"
#include "rtweekend.h"

#include <vector>
#include <string>
#include <cmath>

class image_writer {
  public:
    image_writer(int width, int height) 
        : width(width), height(height), pixels(width * height * 3, 0) {}

    void write_pixel(int x, int y, const color& pixel_color, double samples_scale) {
        auto r = pixel_color.x() * samples_scale;
        auto g = pixel_color.y() * samples_scale;
        auto b = pixel_color.z() * samples_scale;

        // Gamma correction (gamma = 2)
        r = std::sqrt(r);
        g = std::sqrt(g);
        b = std::sqrt(b);

        // Clamp to [0, 0.999] and convert to [0, 255]
        int rbyte = int(256 * std::fmax(0.0, std::fmin(0.999, r)));
        int gbyte = int(256 * std::fmax(0.0, std::fmin(0.999, g)));
        int bbyte = int(256 * std::fmax(0.0, std::fmin(0.999, b)));

        int idx = (y * width + x) * 3;
        pixels[idx + 0] = rbyte;
        pixels[idx + 1] = gbyte;
        pixels[idx + 2] = bbyte;
    }

    bool save_png(const std::string& filename) {
        return stbi_write_png(filename.c_str(), width, height, 3, pixels.data(), width * 3) != 0;
    }

  private:
    int width, height;
    std::vector<unsigned char> pixels;
};

#endif