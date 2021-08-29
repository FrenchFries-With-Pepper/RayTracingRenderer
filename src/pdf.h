#ifndef _PDFH_
#define _PDFH_

#include"vec3.h"
#include"onb.h"
#include"function.h"


class pdf {
public:
	virtual float value(const vec3& direction)const = 0;
	virtual vec3 generate()const = 0;
};

class cosine_pdf :public pdf {
private:
	onb uvw;
	bool is_sphere;
public:
	cosine_pdf(const vec3& w,bool s):is_sphere(s) { uvw.build_from_w(w); }
	virtual float value(const vec3& direction)const {
		float cosine = dot(unit_vector(direction), uvw.w());
		if (cosine > 0)
			return cosine / M_PI;
		else
			return 0;
	}
	virtual vec3 generate()const {
		if (is_sphere)
			return uvw.local(random_cosine_direction());
		else
			return uvw.local(random_in_unit_sphere());
	}
};

class hitable_pdf :public pdf {
private:
	vec3 o;
	std::shared_ptr<hitable> ptr;
public:
	hitable_pdf(std::shared_ptr<hitable> p, const vec3& origin) :ptr(p), o(origin) {}
	virtual float value(const vec3& direction)const {
		return ptr->pdf_value(o, direction);
	}
	virtual vec3 generate()const {
		return ptr->random(o);
	}
};

class mixture_pdf :public pdf {
private:
	std::shared_ptr<pdf> p[2];
public:
	mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1) { p[0] = p0; p[1] = p1; }
	virtual float value(const vec3& direction)const {
		return 0.5*p[0]->value(direction) + 0.5*p[1]->value(direction);
	}
	virtual vec3 generate()const {
		if (get_random() < 0.5)
			return p[0]->generate();
		else
			return p[1]->generate();
	}
};

#endif