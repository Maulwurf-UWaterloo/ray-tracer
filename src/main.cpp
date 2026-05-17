#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "rtweekend.h"
#include "sphere.h"


void add_sphere_line(hittable_list& world,
                     const point3& start,
                     const point3& end,
                     double radius,
                     int count,
                     shared_ptr<material> mat) {
    for (int i = 0; i < count; i++) {
        double t = double(i) / (count - 1);
        point3 pos = start + t * (end - start);
        world.add(make_shared<sphere>(pos, radius, mat));
    }
}

int main() {
    hittable_list world;


    auto ground = make_shared<lambertian>(color(0.85, 0.85, 0.85));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));


    auto w_material = make_shared<metal>(color(1.0, 0.84, 0.2), 0.0);

    double y_top = 2.5;
    double y_bot = 0.4;
    double x_far = 3.0;
    double x_mid = 1.2;
    double y_peak = 1.6;
    double sphere_r = 0.3;
    int per_segment = 12;

    add_sphere_line(world, point3(-x_far, y_top, 0), point3(-x_mid, y_bot, 0),
                    sphere_r, per_segment, w_material);
    add_sphere_line(world, point3(-x_mid, y_bot, 0), point3(0, y_peak, 0),
                    sphere_r, per_segment, w_material);
    add_sphere_line(world, point3(0, y_peak, 0), point3(x_mid, y_bot, 0),
                    sphere_r, per_segment, w_material);
    add_sphere_line(world, point3(x_mid, y_bot, 0), point3(x_far, y_top, 0),
                    sphere_r, per_segment, w_material);


    for (int i = 0; i < 40; i++) {
        double bx = random_double(-7, 7);
        double bz = random_double(-6, -2);
        double by = 0.15 + random_double(0, 0.1);
        auto albedo = color::random() * color::random();
        auto mat = make_shared<lambertian>(albedo);
        world.add(make_shared<sphere>(point3(bx, by, bz), 0.15, mat));
    }


    auto glass = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 0.6, 3), 0.6, glass));

    world = hittable_list(make_shared<bvh_node>(world));


    camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 800;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 30;
    cam.lookfrom = point3(0, 2, 9);
    cam.lookat   = point3(0, 1.4, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0.2;
    cam.focus_dist    = 9.0;

    cam.render(world, "images/signature_W.png");
}