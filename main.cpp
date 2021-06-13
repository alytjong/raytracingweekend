#include "rtweekend.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <iostream>

hittable_list random_scene() {
    //List of object in our scene, stored in world
    hittable_list world;

    // NOTE YOU CANNOT INITIALIZE SHARED POINTERS WITH PREVIOUSLY UNDECLARED
    // VARIABLES SO PLEASE USE THESE
    vec3 object_color;
    vec3 sphere_origin;
    double ground_radius = 1000;
    double sphere_radius = 1.0;
    double small_sphere_radius = 0.2;
    double fuzz;
    double refraction_index;

    // Add ground
    object_color = color(0.5, 0.5, 0.5);
    sphere_origin = point3(0, -1000, 0);
    auto ground_material = make_shared<lambertian>(object_color);
    world.add(make_shared<sphere>(sphere_origin, ground_radius, ground_material));

    // Add random small spheres (random location and materials)
    for (int a = -11; a < 11; a++) {
      for (int b = -11; b < 11; b++) {
        auto choose_mat = random_double();
        point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

        if ((center - point3(4, 0.2, 0)).length() > 0.9) {
          shared_ptr<material> sphere_material;

          // Determine material property based on random number selected
          if (choose_mat < 0.8) {
            // diffuse
            object_color = color::random() * color::random();
            sphere_material = make_shared<lambertian>(object_color);
            world.add(make_shared<sphere>(center, small_sphere_radius, sphere_material));
          } else if (choose_mat < 0.95) {
            // metal
            object_color = color::random(0.5,1);
            fuzz = random_double(0,0.5);
            sphere_material = make_shared<metal>(object_color, fuzz);
            world.add(make_shared<sphere>(center, small_sphere_radius, sphere_material));
          } else {
            // glass
            refraction_index = 1.5;
            sphere_material = make_shared<dielectric>(refraction_index);
            world.add(make_shared<sphere>(center, small_sphere_radius, sphere_material));
          }
        }
      }
    }

    // Add glass sphere
    refraction_index = 1.5;
    sphere_origin = point3(0, 1, 0);
    auto material1 = make_shared<dielectric>(refraction_index);
    world.add(make_shared<sphere>(sphere_origin, sphere_radius, material1));

    // Add diffuse sphere
    object_color = color(0.4, 0.2, 0.1);
    sphere_origin[0] = -4;
    auto material2 = make_shared<lambertian>(object_color);
    world.add(make_shared<sphere>(sphere_origin, sphere_radius, material2));

    // Add metallic sphere
    object_color = color(0.7, 0.6, 0.5);
    sphere_origin[0] = 4;
    fuzz = 0.0;
    auto material3 = make_shared<metal>(object_color, fuzz);
    world.add(make_shared<sphere>(sphere_origin, sphere_radius, material3));

    return world;
}

color ray_color(const ray& r, const hittable& world, int depth) {
    // Output color corresponding to particular pixel
    // depending on what the ray shot from the camera hits
    hit_record rec;

    // Recursion limit for bouncing rays
    if (depth <= 0)
      return color(0,0,0);

    // Only consider hits when it is not directly at the ray source (t > 0)
    if (world.hit(r, 0.001, infinity, rec)) {
      ray scattered;
      color attenuation;

      // If the ray hits a material that scatters light,
      // modify scattered and bounce the scattered light (times attenuation).
      // Otherwise, return black (since light is fully absorbed).
      if (rec.mat_ptr -> scatter(r, rec, attenuation, scattered))
        return attenuation * ray_color(scattered, world, depth-1);
      return color(0,0,0);
    }

    // If no hit, return blue-white background sky
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {

    // Image file
    string filename;
    cout << "Type image file name: ";
    cin >> filename;
    ofstream MyFile(filename);

    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 800;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100; // number of neighboring samples anti-aliasing
    const int max_depth = 50; // maximum times light is allowed to bounce

    // World
    auto world = random_scene();

    // Camera
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1; // large aperture makes more blurry image
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render
    MyFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        cerr << "\rScanlines remaining: " << j << ' ' << flush;
        for (int i = 0; i < image_width; ++i) {
          color pixel_color(0,0,0);
          // anti-aliasing at each pixel by shooting additional rays
          // to the pixel's surroundings and averaging the color
          for (int s = 0; s < samples_per_pixel; ++s) {
            auto u = (i + random_double()) / (image_width - 1);
            auto v = (j + random_double()) / (image_height - 1);
            ray r = cam.get_ray(u,v);
            pixel_color += ray_color(r, world, max_depth);
          }
          write_color(&MyFile, pixel_color, samples_per_pixel);
        }
    }
    cerr << "Done!\n";
    MyFile.close();
}
