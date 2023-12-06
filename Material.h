#pragma once
#include "Ray.h"
#include "Hittable.h"

struct HitRecord;

class Material
{
public:
	virtual bool Scatter(const Ray& r_in, vec3& attenuation, HitRecord& rec, Ray& scattered) const = 0;
	virtual vec3 Emitted(double u, double v, const vec3& p) const;
};

inline vec3 Material::Emitted(double u, double v, const vec3& p) const
{
	return vec3(0, 0, 0);
}
