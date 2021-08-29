#ifndef _RAYH_
#define _RAYH_
#include"vec3.h"
class ray {
private:
	vec3 A;
	vec3 B;
	float _time;
public:

	ray() {}
	ray(const ray& r) {
		A = r.origin();
		B = r.direction();
		_time = r.time();
	}
	ray(const vec3& a, const vec3& b, float t=0) { A = a; B = b; _time = t; }
	vec3 origin()const { return A; }
	vec3 direction()const { return B; }
	vec3 point_at_parameter(float t)const { return A + B * t; }
	float time()const { return _time; }
};

#endif