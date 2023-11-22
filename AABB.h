#pragma once
#include <glm/common.hpp>

#include "Ray.h"

class AABB
{
public:
	AABB() = default;
	AABB(const vec3& a, const vec3& b);

	bool Hit(const Ray& ray, double t_min, double t_max) const;

public:
	vec3 minmum;
	vec3 maxmum;
};

inline AABB::AABB(const vec3& minnum, const vec3& maxnum) : minmum(minmum), maxmum(maxmum)
{
}

inline bool AABB::Hit(const Ray& ray, double t_min, double t_max) const
{
	for (int i = 0; i < 3; i++)
	{
		const auto invD = 1.0 / ray.dir[i];
		auto t0 = (minmum[i] - ray.orig[i]) * invD;
		auto t1 = (maxmum[i] - ray.orig[i]) * invD;
		if (invD < 0.0)
		{
			std::swap(t0, t1);
		}

		t_min = t0 > t_min ? t0 : t_min;
		t_max = t1 < t_max ? t1 : t_max;

		if (t_max <= t_min)
		{
			return false;
		}
	}

	return true;
}
