#include <iostream>
#include <fstream>

int main() {
    int width = 256;
    int height = 256;
    
    std::ofstream out("output.ppm");
    out << "P3\n" << width << " " << height << "\n255\n";
    
    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            int r = (i * 255) / width;      // left to right: black to red
            int g = (j * 255) / height;     // bottom to top: black to green
            int b = 0;
            out << r << " " << g << " " << b << "\n";
        }
    }
    
    std::cout << "Wrote output.ppm\n";
    return 0;
}