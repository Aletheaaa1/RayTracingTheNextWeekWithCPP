#pragma once
#include "Material.h"
#include "Ray.h"

class Lambertian : public Material
{
public:
	Lambertian() = default;
	Lambertian(vec3 a);

	bool Scatter(const Ray& r_in, vec3& attenuation, HitRecord& rec, Ray& scattered) const override;

	vec3 albedo;
};

inline Lambertian::Lambertian(vec3 a) :albedo(a)
{
}

inline bool Lambertian::Scatter(const Ray& r_in, vec3& attenuation, HitRecord& rec, Ray& scattered) const
{
	const vec3 dir = rec.normal + random_unit_vector();
	scattered = Ray{ rec.p, dir, r_in.time };
	attenuation = albedo;

	return true;
}
