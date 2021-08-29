#ifndef _MATERIALH_
#define _MATERIALH_

#include<cstdlib>
#include<random>
#include"hitable.h"
#include"texture.h"
#include"onb.h"
#include"pdf.h"
#include"function.h"

//vec3 random_in_unit_sphere() {
//	vec3 p;
//	do {
//		p = 2.0*vec3(get_random(), get_random(), get_random()) - vec3(1, 1, 1);
//	} while (p.squared_length() >= 1.0f);
//	return p;
//}

struct scatter_record {
	ray specular_ray;
	bool is_specular;
	vec3 attenuation;
	std::shared_ptr<pdf>  pdf_ptr;
};

class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& hrec,scatter_record& srec)const { return false; };
	virtual float scattering_pdf(const ray & ray_in, const hit_record& rec, ray& scattered)const { return false; }
	virtual vec3 emitted(const ray& r_in,const hit_record& rec,float u, float v, const vec3& p)const { return vec3(0, 0, 0); }
};

class lambertian :public material {
private:
	std::shared_ptr<texture>  albedo;
public:
	lambertian(std::shared_ptr<texture>  a) :albedo(a) {}
	float scattering_pdf(const ray& ray_in, const hit_record& rec, ray& scattered)const {
		float cos = dot(rec.normal, unit_vector(scattered.direction()));
		if(cos < 0) cos = 0;
		return cos / M_PI;
	}

	bool scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec)const {
		srec.is_specular = false;
		srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.p);
		srec.pdf_ptr =std::make_shared<cosine_pdf>(hrec.normal,false);//cosine_pdf class covered the codes below
		//onb uvw;
		//uvw.build_from_w(rec.normal);
		//do {
		//	vec3 direction = uvw.local(random_cosine_direction());
		//	//vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		//	//scattered = ray(rec.p, target - rec.p, r_in.time());
		//	scattered = ray(rec.p, unit_vector(direction), r_in.time());
		//	pdf = dot(uvw.w(), unit_vector(scattered.direction())) / M_PI;//cos(theta)/Pi
		//} while (pdf == 0);
		//attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
};

vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;
}

class metal :public material {
private:
	vec3 albedo;
	float fuzz;
public:
	metal(const vec3& a,float f) :albedo(a) {
		if (f < 1)fuzz = f;
		else fuzz = 1;
	}
	virtual bool scatter(const ray& r_in, const hit_record& hrec,  scatter_record& srec)const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), hrec.normal);
		srec.specular_ray = ray(hrec.p, reflected + fuzz * random_in_unit_sphere(),r_in.time());
		srec.attenuation = albedo;
		srec.is_specular = true;
		srec.pdf_ptr = 0;
		return true;
		//scattered = ray(rec.p, reflected+fuzz*random_in_unit_sphere(),r_in.time());
		//attenuation = albedo;
		//return (dot(scattered.direction(), rec.normal) > 0);
	}
};

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);//cos(theta)
	float discriminant = 1.0 - ni_over_nt * ni_over_nt*(1 - dt * dt);//squared cos(theta2)
	if (discriminant > 0) {//cos(theta2)>0 refraction
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else//cos(theta2)<0 total internal reflection
		return false;
}

float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}

class dielectric :public material {
public:
	float ref_idx;
	dielectric(float ri) :ref_idx(ri) {}
	virtual bool scatter(const ray& r_in, const hit_record& hrec,scatter_record& srec)const {
		srec.is_specular = true;
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), hrec.normal);
		float ni_over_nt;
		srec.attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		float reflect_prob;
		float cosine;
		if (dot(r_in.direction(), hrec.normal) > 0) {//the ray coming from inside
			outward_normal = -hrec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx * dot(r_in.direction(), hrec.normal) / r_in.direction().length();
		}
		else {
			outward_normal = hrec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), hrec.normal) / r_in.direction().length();
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_prob = schlick(cosine, ref_idx);
		}
		else {
			srec.specular_ray = ray(hrec.p, reflected,r_in.time());
			reflect_prob = 1.0;
		}
		if (get_random() < reflect_prob) {
			srec.specular_ray = ray(hrec.p, reflected,r_in.time());
		}
		else {
			srec.specular_ray = ray(hrec.p, refracted,r_in.time());
		}
		return true;
	}
};


class isotropic :public material {
private:
	std::shared_ptr<texture> albedo;
public:
	isotropic(texture* a) :albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& hrec, scatter_record& srec)const {
		srec.is_specular = false;
		srec.attenuation = albedo->value(hrec.u,hrec.v,hrec.p);
		srec.pdf_ptr =std::make_shared<cosine_pdf>(hrec.normal, true);
		//scattered = ray(rec.p, random_in_unit_sphere(), rec.t);
		//attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
};

#endif