#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include"vec3.h"
#define _USE_MATH_DEFINES
#include<math.h>

float get_random() {
	float rand = std::rand() / (float)RAND_MAX;
	return rand;
}

vec3 random_in_unit_sphere() {
	float r1 = get_random();
	float r2 = get_random();
	float phi = 2 * M_PI*r1;
	float z = 1 - 2 * r2;
	float x = cos(phi) * 2 * sqrt(r2 * (1 - r2));
	float y = sin(phi) * 2 * sqrt(r2 * (1 - r2));
	return vec3(x, y, z);
}

inline vec3 random_cosine_direction() {
	float r1 = get_random();
	float r2 = get_random();
	float z = sqrt(1 - r2);
	float phi = 2 * M_PI*r1;
	float x = cos(phi) * 2 * sqrt(r2);
	float y = sin(phi) * 2 * sqrt(r2);
	return vec3(x, y, z);
}

inline vec3 random_to_sphere(float radius, float distance_squared) {
	float r1 = get_random();
	float r2 = get_random();
	float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
	float phi = 2 * M_PI*r1;
	float x = cos(phi)*sqrt(1 - z * z);
	float y = sin(phi)*sqrt(1 - z * z);
	return vec3(x, y, z);
}

#endif
