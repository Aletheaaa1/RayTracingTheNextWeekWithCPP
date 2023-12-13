#pragma once
#include "material.h"
#include "ray.h"
#include "solid_color.h"
#include "checker_texture.h"
#include "texture.h"

class Lambertian : public Material
{
public:
	Lambertian() = default;
	Lambertian(vec3 a);
	Lambertian(std::shared_ptr<Texture> a);

	bool Scatter(const Ray& r_in, vec3& attenuation, HitRecord& rec, Ray& scattered) const override;

public:
	std::shared_ptr<Texture> albedo;
};

inline Lambertian::Lambertian(vec3 a) :albedo(std::make_shared<SolidColor>(a))
{
}

inline Lambertian::Lambertian(std::shared_ptr<Texture> a) : albedo(std::move(a))
{
}

inline bool Lambertian::Scatter(const Ray& r_in, vec3& attenuation, HitRecord& rec, Ray& scattered) const
{
	const vec3 dir = rec.normal + random_unit_vector();
	scattered = Ray{ rec.p, dir, r_in.time };
	attenuation = albedo->Value(rec.u, rec.v, rec.p);

	return true;
}
