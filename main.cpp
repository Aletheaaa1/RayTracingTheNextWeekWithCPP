#include <chrono>
#include <iostream>
#include <glm/simd/platform.h>

#include "Vec3.h"
#include "Camera.h"
#include "Ray.h"
#include "HittableList.h"
#include "Sphere.h"

#include "Lambertian.h"
#include "Metallic.h"
#include "Dielectric .h"
#include "MovingSphere.h"

void WriteColor(std::ostream& out, color pixel_color)
{
	// Write the translated [0,255] value of each color component.
	out << static_cast<int>(255.999 * pow(pixel_color.x(), 1.0 / 2.2)) << ' '
		<< static_cast<int>(255.999 * pow(pixel_color.y(), 1.0 / 2.2)) << ' '
		<< static_cast<int>(255.999 * pow(pixel_color.z(), 1.0 / 2.2)) << '\n';
}

vec3 RayColor(const Ray& ray, const HittableList& world, int deepth)
{
	if (deepth < 0)
	{
		return vec3(0, 0, 0);
	}

	constexpr double t_min = 0.000001;
	constexpr double t_max = 1000000.0;
	HitRecord rec;

	if (world.Hit(ray, t_min, t_max, rec))
	{
		vec3 attenuation(0, 0, 0);
		Ray scattered;
		if (rec.mat_ptr->Scatter(ray, attenuation, rec, scattered))
		{
			return RayColor(scattered, world, deepth - 1) * attenuation;
		}
	}

	vec3 unit_direction = normalize(ray.dir);
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

HittableList RandomWorld()
{
	HittableList world;
	auto ground_material = std::make_shared<Lambertian>(color(0.5, 0.5, 0.5));
	world.Add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				std::shared_ptr<Material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = std::make_shared<Lambertian>(albedo);
					world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// Metallic
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = std::make_shared<Metallic>(albedo, fuzz);
					world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = std::make_shared<Dielectric>(1.5);
					world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	// ≤£¡ß¥Û«Ú
	auto material1 = std::make_shared<Dielectric>(1.5);
	world.Add(std::make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

	// …¢…‰¥Û«Ú
	auto material2 = std::make_shared<Lambertian>(color(0.4, 0.2, 0.1));
	world.Add(std::make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

	// Ω Ù¥Û«Ú
	auto material3 = std::make_shared<Metallic>(color(0.7, 0.6, 0.5), 0.0);
	world.Add(std::make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}

int main() {
	auto start_time = std::chrono::high_resolution_clock::now();

	constexpr double aspect = 16.0 / 9.0;
	constexpr int image_height = 720;
	constexpr int image_width = image_height * aspect;

	int deepth = 50;
	int samplerNum;
	std::cin >> samplerNum;

	Camera camera(40, aspect, vec3(3, 3, 2), vec3(0, 0, -1.0), vec3(0, 1.0, 0), 0.0, 1.0);

	// World
	//HittableList world = RandomWorld();
	HittableList world;

	auto material_ground = std::make_shared<Lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<Lambertian>(color(0.7, 0.3, 0.3));
	auto material_left = std::make_shared<Dielectric>(1.5);
	auto material_right = std::make_shared<Metallic>(vec3(0.7, 0.7, 0.1), 0.0);

	world.Add(std::make_shared<MovingSphere>(point3(-2.0, 0, -1.0), point3(-2.0, 1.0, -1.0), 0.0, 1.0, 0.5, material_center));
	world.Add(std::make_shared<Sphere>(point3(0, 0, -1.0), 0.5, material_center));
	world.Add(std::make_shared<Sphere>(point3(-1.0, 0, -1.0), 0.5, material_left));
	world.Add(std::make_shared<Sphere>(point3(-1.0, 0, -1.0), -0.4, material_left));
	world.Add(std::make_shared<Sphere>(point3(1.0, 0, -1.0), 0.5, material_right));
	world.Add(std::make_shared<Sphere>(point3(0, -1000.5, -1.0), 1000, material_ground));

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cerr << "\rRemaining " << j << " lines..." << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			vec3 color(0.0, 0.0, 0.0);

			for (int k = 0; k < samplerNum; k++)
			{
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);

				Ray r = camera.GetRay(u, v);

				color += RayColor(r, world, deepth);
			}

			color /= samplerNum;
			WriteColor(std::cout, color);
		}
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cerr << "\nFinish! using " << duration << std::endl;
	system("pause");
}