#ifndef _HITABLEH_
#define _HITABLEH_

#include"ray.h"
#include"aabb.h"
class material;
	
aabb surrounding_box(aabb box0, aabb box1) {
	vec3 small(fmin(box0.min().x(), box1.min().x()),
		fmin(box0.min().y(), box1.min().y()),
		fmin(box0.min().z(), box1.min().z()));
	vec3 big(fmax(box0.max().x(), box1.max().x()),
		fmax(box0.max().y(), box1.max().y()),
		fmax(box0.max().z(), box1.max().z()));
	return aabb(small, big);
}

struct hit_record {
	float t;
	vec3 p;
	vec3 normal;
	material *mate_ptr;
	float u;
	float v;
};

class hitable {
public:
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)const = 0;
	virtual bool bounding_box(float t0, float t1, aabb& box)const = 0;
	virtual float pdf_value(const vec3& o, const vec3& v)const { return 0.0; }
	virtual vec3 random(const vec3& o)const { return vec3(1, 0, 0); }
};

#endif