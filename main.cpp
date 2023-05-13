#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

#include <iostream>
// sphere
// 计算光线和圆的交汇点，判断光线和圆的碰撞
// 这一部分的代码移到了 shpere.h
// double hit_sphere(const point3& center, double radius, const ray& r) {
//     vec3 oc = r.origin() - center;
//     auto a = r.direction().length_squared();
//     // 减少计算开销 利用 b = 2h
//     // auto b = 2.0 * dot(oc, r.direction());
//     auto half_b = dot(oc, r.direction());
//     // auto c = dot(oc, oc) - radius*radius;
//     auto c = oc.length_squared() - radius*radius;
//     // auto discriminant = b*b - 4*a*c;
//     auto discriminant = half_b*half_b - a*c;
//     if (discriminant < 0) {
//         return -1;
//     } else {
//         // return (-b  - sqrt(discriminant)) / (2.0*a);
//         return (-half_b - sqrt(discriminant)) / a;
//     }
// }




color ray_color(const ray& r, const hittable& world, int depth) {
    // if (hit_sphere(point3(0, 0, 1), 0.5, r)) return color(1, 0, 0);
    // auto t = hit_sphere(point3(0, 0, -1), 0.5, r);

    hit_record rec;
    
    if (depth <= 0) { // 如果超过depth， 则认为光能已经耗尽，不再反射
        return color(0, 0, 0);
    }
    
    if (world.hit(r, 0.001, infinity, rec)) { // 如果有碰撞
        // vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1)); // 
        // return 0.5*color(N.x()+1, N.y()+1, N.z()+1);
        
        // 加入漫反射
        // point3 target = rec.p + rec.normal + random_in_uint_sphere();
        // point3 target = rec.p + rec.normal + random_unit_vector();
        // point3 target = rec.p + random_in_hemisphere(rec.normal);
        // return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);

        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, world, depth-1);
        }
        return color(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}


hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.0) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0 , material1));
    
    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0 , material2));
    
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0 , material3));

    return world;
}

void test1() {
    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    auto R = cos(pi / 4);
    hittable_list world;
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    // auto material_center = make_shared<dielectric>(1.5);
    auto material_left = make_shared<dielectric>(1.5);
    // auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

   
    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    // Camera
    point3 lookfrom(3, 3, 2);
    point3 lookat(0, 0, -1);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = (lookfrom - lookat).length();
    auto aperture = 2.0;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
    // auto viewport_height = 2.0;
    // auto viewport_width = aspect_ratio * viewport_height;
    // auto focal_length = 1.0;

    // auto origin = point3(0, 0, 0);
    // auto horizontal = vec3(viewport_width, 0, 0);
    // auto vertical = vec3(0, viewport_height, 0);
    // auto low_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

    // Render
    std::cout << "P3 \n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) { //  The rows are written out from top to bottom
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) { // The pixels are written out in rows with pixels left to right
            // auto r = double(i) / (image_width-1);
            // auto g = double(j) / (image_height-1);
            // auto b = 0.25;

            // int ir = static_cast<int>(255.99 * r);
            // int ig = static_cast<int>(255.99 * g);
            // int ib = static_cast<int>(255.99 * b);

            // std::cout << ir << ' ' << ig << ' ' << ib << "\n";

            color pixel_color(0, 0, 0);
            for (int  s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            
            // ray r(origin, low_left_corner + u*horizontal + v*vertical - origin);
            // color pixel_color = ray_color(r, world);
            
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}


void test2() {
    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);  
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    // world
    auto world = random_scene();

    // camera
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render
    std::cout << "P3 \n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) { //  The rows are written out from top to bottom
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) { // The pixels are written out in rows with pixels left to right
            color pixel_color(0, 0, 0);
            for (int  s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}

int main() {
    test2();

    return 0;
}