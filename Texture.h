#pragma once
#include "Vec3.h"
void A()
{}
class Texture
{
public:
	virtual vec3 Value(double u, double v, const vec3& position) const = 0;
};
