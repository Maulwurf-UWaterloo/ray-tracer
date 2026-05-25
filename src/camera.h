#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "hittable.h"
#include "material.h"    
#include "rtweekend.h"
#include "image_writer.h"
#include <string>
#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>

class camera {
  public:
    double aspect_ratio = 1.0;
    int    image_width  = 100;
    int    samples_per_pixel = 10;
    int    max_depth = 10;
    double vfov = 90;                     
    point3 lookfrom = point3(0,0,0);      
    point3 lookat   = point3(0,0,-1);     
    vec3   vup      = vec3(0,1,0); 
    double defocus_angle = 0;    
    double focus_dist    = 10;   
    
void render(const hittable& world, const std::string& output_filename = "output.png") {
    initialize();

    auto start = std::chrono::high_resolution_clock::now();

    image_writer img(image_width, image_height);


    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;


    const int tile_size = 32;
    const int tiles_x = (image_width + tile_size - 1) / tile_size;
    const int tiles_y = (image_height + tile_size - 1) / tile_size;
    const int total_tiles = tiles_x * tiles_y;


    std::atomic<int> next_tile{0};

    std::clog << "Rendering " << image_width << "x" << image_height 
              << " with " << num_threads << " threads, " 
              << total_tiles << " tiles...\n";

    auto worker = [&]() {
        while (true) {
            int tile_idx = next_tile.fetch_add(1);
            if (tile_idx >= total_tiles) break;

            int tile_x = tile_idx % tiles_x;
            int tile_y = tile_idx / tiles_x;

            int x_start = tile_x * tile_size;
            int y_start = tile_y * tile_size;
            int x_end = std::min(x_start + tile_size, image_width);
            int y_end = std::min(y_start + tile_size, image_height);

            for (int j = y_start; j < y_end; j++) {
                for (int i = x_start; i < x_end; i++) {
                    color pixel_color(0, 0, 0);
                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    img.write_pixel(i, j, pixel_color, pixel_samples_scale);
                }
            }
        }
    };


    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (unsigned int t = 0; t < num_threads; t++) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    img.save_png(output_filename);

    std::clog << "Done in " << duration.count() << " seconds using " 
              << num_threads << " threads. Saved to " << output_filename << "\n";
  }

  private:
    int    image_height;
    double pixel_samples_scale;
    point3 center;
    point3 pixel00_loc;
    vec3   pixel_delta_u;
    vec3   pixel_delta_v;
    vec3   u, v, w;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;
    
    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;
        
        center = lookfrom;
        
        auto focal_length = focus_dist;
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focal_length;
        auto viewport_width = viewport_height * (double(image_width)/image_height);
        
        auto w = unit_vector(lookfrom - lookat);   
        auto u = unit_vector(cross(vup, w));        
        auto v = cross(w, u);                       
        
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;
        
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        
        auto viewport_upper_left = center - (focal_length * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }
    
    ray get_ray(int i, int j) const {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                      + ((i + offset.x()) * pixel_delta_u)
                      + ((j + offset.y()) * pixel_delta_v);
                      
                      auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
                      auto ray_direction = pixel_sample - ray_origin;
                      
                      return ray(ray_origin, ray_direction);
                    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

   point3 defocus_disk_sample() const {      
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        if (depth <= 0)
        return color(0,0,0);
        
        hit_record rec;
        
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, depth-1, world);
            return color(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
};

#endif