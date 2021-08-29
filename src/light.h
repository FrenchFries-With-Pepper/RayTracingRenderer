#ifndef _light_h_
#define _light_h_

#include"material.h"

class diffuse_light :public material {
private:
	std::shared_ptr<texture>  emit;
public:
	diffuse_light(std::shared_ptr<texture> _emit) :emit(_emit) {}	
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scatterd)const { return false; }
	virtual vec3 emitted(const ray& r_in,const hit_record& rec,float u, float v, const vec3& p)const { 
		if (dot(rec.normal, r_in.direction()) < 0.0)
			return emit->value(u, v, p);
		else
			return vec3(0, 0, 0);
	}
};
#endif