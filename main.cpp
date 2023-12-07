#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <omp.h>

#include "Box.h"
#include "BVH.h"
#include "Vec3.h"
#include "Camera.h"
#include "Ray.h"
#include "HittableList.h"
#include "Sphere.h"

#include "Lambertian.h"
#include "Metallic.h"
#include "Dielectric .h"
#include "DiffuseLight.h"
#include "MovingSphere.h"
#include "Rect.h"

void WriteColor(std::ostream& out, color pixel_color)
{
	// Write the translated [0,255] value of each color component.
	out << static_cast<int>(255.999 * pow(pixel_color.x() / (pixel_color.x() + 1), 1.0 / 2.2)) << ' '
		<< static_cast<int>(255.999 * pow(pixel_color.y() / (pixel_color.y() + 1), 1.0 / 2.2)) << ' '
		<< static_cast<int>(255.999 * pow(pixel_color.z() / (pixel_color.z() + 1), 1.0 / 2.2)) << '\n';
}

vec3 RayColor(const Ray& ray, const Hittable& world, int deepth, vec3 background)
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
		vec3 emit = rec.mat_ptr->Emitted(rec.u, rec.v, rec.p);
		if (rec.mat_ptr->Scatter(ray, attenuation, rec, scattered))
		{
			return RayColor(scattered, world, deepth - 1, background) * attenuation + emit;
		}
		return emit;
	}

	return background;
}

void RandomWorld(HittableList& world)
{
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

	// ��������
	auto material1 = std::make_shared<Dielectric>(1.5);
	world.Add(std::make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

	// ɢ�����
	auto material2 = std::make_shared<Lambertian>(color(0.4, 0.2, 0.1));
	world.Add(std::make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

	// ��������
	auto material3 = std::make_shared<Metallic>(color(0.7, 0.6, 0.5), 0.0);
	world.Add(std::make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));
}
void DefaultWorld(HittableList& world)
{
	auto material_ground = std::make_shared<Lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<Lambertian>(color(0.7, 0.3, 0.3));
	auto material_left = std::make_shared<Dielectric>(1.5);
	auto material_right = std::make_shared<Metallic>(vec3(0.3, 0.5, 0.6), 0.0);

	//world.Add(std::make_shared<MovingSphere>(point3(-2.0, 0, -1.0), point3(-2.0, 1.0, -1.0), 0.0, 1.0, 0.5, material_center));
	world.Add(std::make_shared<Sphere>(point3(0, 0, -1.0), 0.5, material_center));
	world.Add(std::make_shared<Sphere>(point3(-1.0, 0, -1.0), 0.5, material_left));
	world.Add(std::make_shared<Sphere>(point3(-1.0, 0, -1.0), -0.4, material_left));
	world.Add(std::make_shared<Sphere>(point3(1.0, 0, -1.0), 0.5, material_right));
	world.Add(std::make_shared<Sphere>(point3(0, -1000.5, -1.0), 1000, material_ground));
}
void SimpleWorld(HittableList& world)
{
	auto light = std::make_shared<DiffuseLight>(vec3(4, 4, 4));
	auto material_ground = std::make_shared<Lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<Lambertian>(color(0.7, 0.3, 0.3));

	world.Add(std::make_shared<Sphere>(point3(0, -1000.5, -1.0), 1000, material_ground));
	world.Add(std::make_shared<Sphere>(point3(0, 2, 0), 2, material_center));
	world.Add(std::make_shared<Sphere>(point3(0, 6, 0), 1, light));
	world.Add(std::make_shared<XYRect>(3, 5, 1, 3, -2, light));
}
void CornellBox(HittableList& world)
{
	auto red = std::make_shared<Lambertian>(color(.65, .05, .05));
	auto white = std::make_shared<Lambertian>(color(.73, .73, .73));
	auto green = std::make_shared<Lambertian>(color(.12, .45, .15));
	auto light = std::make_shared<DiffuseLight>(color(20, 20, 20));

	world.Add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
	world.Add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
	world.Add(std::make_shared<XZRect>(213, 343, 227, 332, 554, light));
	world.Add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
	world.Add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
	world.Add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));

	world.Add(std::make_shared<Box>(point3(130, 0, 65), point3(295, 165, 230), white));
	world.Add(std::make_shared<Box>(point3(265, 0, 295), point3(430, 330, 460), white));
}

int main()
{
#pragma region Camera
	constexpr double aspect = 16.0 / 9.0;
	constexpr int image_height = 720;
	constexpr int image_width = image_height * aspect;
	constexpr int depth = 100;
	int samplerNum;
	std::cin >> samplerNum;

	Camera camera(40, aspect, vec3(278, 278, -800), vec3(278, 278, 0.0), vec3(0, 1.0, 0), 0.0, 1.0);
#pragma  endregion Camera

#pragma region World
	// World
	HittableList world;
	//DefaultWorld(world);
	//SimpleWorld(world);
	CornellBox(world);
	//RandomWorld(world);
	const auto start_time = std::chrono::high_resolution_clock::now();
	const BVH bvh_node{ world };
#pragma endregion World

#pragma region Rendering

	std::vector<vec3> colors(image_width * image_height);
	omp_set_num_threads(8);

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	//omp_set_num_threads(std::thread::hardware_concurrency());
#pragma omp parallel for schedule(dynamic, 100)
	for (int j = image_height - 1; j >= 0; --j)
	{
		std::cerr << "\rRemaining " << j << " lines..." << std::flush;
		for (int i = 0; i < image_width; ++i)
		{
			vec3 color(0.0, 0.0, 0.0);

			for (int k = 0; k < samplerNum; k++)
			{
				const auto u = (i + random_double()) / (image_width - 1);
				const auto v = (j + random_double()) / (image_height - 1);

				Ray r = camera.GetRay(u, v);

				color += RayColor(r, bvh_node, depth, vec3(0, 0, 0));
			}

			color /= samplerNum;
#pragma omp critical
			colors[j * image_width + i] = color;
		}
	}

	for (auto i = colors.end() - 1; i >= colors.begin(); --i)
	{
		WriteColor(std::cout, *i);
	}
#pragma endregion Rendering

	const auto end_time = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cerr << "\nFinish! using " << duration << std::endl;

	system("pause");
}