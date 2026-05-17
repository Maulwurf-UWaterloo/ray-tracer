# Ray Tracer

A path-traced ray tracer in C++17, built from scratch following the *Ray Tracing in One Weekend* series.

![Cover render](images/final_render.png)

## Features

- [x] Lambertian (diffuse) material
- [x] Metal material with adjustable fuzz
- [x] Dielectric (glass) material with Schlick reflectance
- [x] Anti-aliasing via stochastic sampling
- [x] Positionable camera with vertical FOV
- [x] Defocus blur (depth of field)
- [x] PNG output via stb_image_write
- [x] BVH acceleration structure (median-split, longest-axis)
- [ ] Multi-threaded rendering (in progress)

## Build

Requires CMake 3.15+ and a C++17 compiler.

```bash
cmake -B build
cmake --build build
./build/ray_tracer
```

Output is saved as `images/final_render.png`.

## Architecture

Header-only components in `src/`:

| File | Responsibility |
|------|----------------|
| `vec3.h` | 3D vector math with operator overloads |
| `ray.h` | Parameterized ray (origin + direction) |
| `interval.h` | 1D interval utility for ray-bounds tests |
| `aabb.h` | Axis-Aligned Bounding Box with slab-method intersection |
| `hittable.h` | Abstract base for intersectable objects |
| `sphere.h` | Sphere primitive with quadratic intersection |
| `hittable_list.h` | Composite of multiple hittables |
| `bvh.h` | Bounding Volume Hierarchy for O(log n) traversal |
| `material.h` | Lambertian, Metal, Dielectric materials |
| `camera.h` | Positionable camera with defocus blur and timing instrumentation |
| `image_writer.h` | PNG output with gamma correction |

## Performance

Benchmark scene: 488 randomly placed spheres (RTIOW final scene), 400×225, 100 samples/pixel, max depth 50.
Hardware: 1.4 GHz Quad-Core Intel Core i5 (2020 MacBook Pro 13").

| Configuration | Render Time | Speedup |
|---------------|-------------|---------|
| Single-threaded, no BVH (Release) | 60 s | 1.0× (baseline) |
| Single-threaded, with BVH | 14 s | **4.3×** |
| Multi-threaded, with BVH | TBD | TBD |

## References

- *[Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)* by Peter Shirley
- *[Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html)* by Peter Shirley (BVH chapter)