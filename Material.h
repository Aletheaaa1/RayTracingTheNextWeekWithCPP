#pragma once
#include "Ray.h"
#include "Hittable.h"

struct HitRecord;

class Material
{
public:
	virtual bool Scatter(const Ray& r_in, vec3& attenuation, HitRecord& rec, Ray& scattered) const = 0;
};
