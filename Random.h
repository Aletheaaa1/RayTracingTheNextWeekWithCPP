#pragma once
#include <random>
#include <glm/glm.hpp>

constexpr double PI = 3.1415926;

class Random
{
public:
	static double Randomdouble();
	static double Randomdouble(double min, double max);
	static glm::dvec3 RandomInDist();
	static glm::dvec3 RandomInFace();
};

inline double Random::Randomdouble()
{
	return rand() / (RAND_MAX + 1.0);
}

inline double Random::Randomdouble(double min, double max)
{
	return min + Randomdouble() * (max - min);
}

inline glm::dvec3 Random::RandomInDist()
{
	while (true)
	{
		auto p = glm::dvec3(Randomdouble(-1.0f, 1.0f), Randomdouble(-1.0f, 1.0f), Randomdouble(-1.0f, 1.0f));
		if (glm::length(p) > 1.0f) continue;
		return p;
	}
}

inline glm::dvec3 Random::RandomInFace()
{
	double theta = Randomdouble() * 2.0f * PI;
	double phi = Randomdouble() * PI;

	glm::dvec3 p;
	p.y = cos(phi);
	p.x = sin(phi) * cos(theta);
	p.z = sin(phi) * sin(theta);

	return p;
}
