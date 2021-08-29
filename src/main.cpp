#include"ray.h"
#include<iostream>
#include<fstream>
#include"hitablelist.h"
#include<float.h>
#include"sphere.h"
#include<cstdlib>
#include<ctime>
#include<random>
#include"camera.h"
#include"material.h"
#include"aabb.h"
#include"bvh_node.h"
#include"texture.h"
#include"rectangle.h"
#include"light.h"
#include"box.h"
#include"translate.h"
#include"rotate.h"
#include"volume.h"
#include"pdf.h"

#define STB_IMAGE_IMPLEMENTATION 
#include<stb_image.h>

vec3 color(const ray& r, hitable* world, std::shared_ptr<hitable> light_shape, int depth) {
	hit_record hrec;
	if (world->hit(r, 0.0001f, FLT_MAX, hrec)) {
		scatter_record srec;

		vec3 emitted = hrec.mate_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
		if (depth < 50 && hrec.mate_ptr->scatter(r, hrec, srec)) {
			if (srec.is_specular)
				return emitted + color(srec.specular_ray, world, light_shape, depth + 1);
			else {
				hitable_pdf plight(light_shape, hrec.p);
				mixture_pdf p(std::make_shared<hitable_pdf>(plight), srec.pdf_ptr);
				float pdf_val;
				ray scattered;
				do {
					scattered = ray(hrec.p, p.generate(), r.time());
					pdf_val = p.value(scattered.direction());
				} while (pdf_val == 0);
				return emitted + srec.attenuation * hrec.mate_ptr->scattering_pdf(r, hrec, scattered)
					* color(scattered, world, light_shape, depth + 1) / pdf_val;
			}
		}
		else
			return emitted;
	}
	else {
		return vec3(0, 0, 0);
	}
}

hitable* cornellbox(int nx, int ny, camera& cam) {
	hitable** list = new hitable * [8];
	int i = 0;
	material* red = new lambertian(std::make_shared<constant_texture>(constant_texture(vec3(0.65, 0.05, 0.05))));
	material* white = new lambertian(std::make_shared<constant_texture>(constant_texture(vec3(0.73, 0.73, 0.73))));
	material* green = new lambertian(std::make_shared<constant_texture>(constant_texture(vec3(0.12, 0.45, 0.15))));
	material* light = new diffuse_light(std::make_shared<constant_texture>(constant_texture(vec3(16, 16, 16))));
	material* aluminum = new metal(vec3(0.8, 0.85, 0.88), 0.0);
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new flip_normals(new xz_rect(213.f, 343.f, 227.f, 332.f, 554.f, light));
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0.f, 555.f, 0.f, 555.f, 0.f, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	cam = camera(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);
	return new hitable_list(list, i);
}

//hitable * two_perlin_spheres(int nx, int ny, camera& cam) {
//	std::shared_ptr<texture> pertext = std::make_shared<noise_texture>(noise_texture(5.f));
//	hitable **list = new hitable*[1];
//	//list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
//	list[0] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
//	vec3 lookfrom(13, 2, 3);
//	vec3 lookat(0, 0, 0);
//	float dist_to_focus = 10.0;
//	float aperture = 0.0;
//	cam = camera(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);
//	return new hitable_list(list, 1);
//}
//
hitable *cornell_smoke(int nx, int ny, camera& cam) {
	hitable**list = new hitable*[8];
	int i = 0;
	material* red = new lambertian(std::make_shared<constant_texture>(constant_texture(vec3(0.65, 0.05, 0.05))));
	material* white = new lambertian(std::make_shared<constant_texture>(constant_texture(vec3(0.73, 0.73, 0.73))));
	material* green = new lambertian(std::make_shared<constant_texture>(constant_texture(vec3(0.12, 0.45, 0.15))));
	material* light = new diffuse_light(std::make_shared<constant_texture>(constant_texture(vec3(16, 16, 16))));
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	hitable *b1 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	hitable *b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	list[i++] = new constant_medium(b1, 0.9, new constant_texture(vec3(1.0, 1.0, 1.0)));
	list[i++] = new constant_medium(b2, 0.1, new constant_texture(vec3(0.0, 0.0, 0.0)));
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	cam = camera(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);
	return new hitable_list(list, i);
}
//hitable *cornell_smoke(int nx, int ny, camera& cam) {
//	hitable**list = new hitable*[8];
//	int i = 0;
//	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
//	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
//	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
//	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
//	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
//	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
//	list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
//	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
//	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
//	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
//	hitable *b1 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
//	hitable *b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
//	list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
//	list[i++] = new constant_medium(b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));
//	vec3 lookfrom(278, 278, -800);
//	vec3 lookat(278, 278, 0);
//	float dist_to_focus = 10.0;
//	float aperture = 0.0;
//	float vfov = 40.0;
//	cam = camera(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);
//	return new hitable_list(list, i);
//	return new hitable_list(list, i);
//}

//hitable * final(int nx, int ny, camera& cam) {
//	int nb = 20;
//	hitable **list = new hitable*[30];
//	hitable **boxlist = new hitable*[10000];
//	hitable **boxlist2 = new hitable*[10000];
//	material* white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
//	material* ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
//	//int b = 0;
//	//for (int i = 0; i < nb; i++) {
//	//	for (int j = 0; j < nb; j++) {
//	//		float w = 100;
//	//		float x0 = -1000 + i * w;
//	//		float y0 = 0;
//	//		float z0 = -1000 + j * w;
//	//		float x1 = x0 + w;
//	//		float y1 = 100 * (get_random() + 0.01);
//	//		float z1 = z0 + w;
//	//		boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
//	//	}
//	//}
//	int l = 0;
//	//list[l++] = new bvh_node(boxlist, b, 0, 1);
//	//material*light = new diffuse_light(new constant_texture(vec3(8, 8, 8)));
//	//list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
//	//vec3 center(400, 400, 200);
//	//list[l++] = new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
//	//list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
//	//list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));
//	//hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
//	//list[l++] = boundary;
//	//list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2,0.4,0.9)));
//	//boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
//	//list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));
//	//int nx, ny, nn;
//	//unsigned char *tex_data = stbi_load("C:\\Users\\HJW\\Desktop\\timg.jpg", &nx, &ny, &nn, 0);
//	//material *emat = new lambertian(new image_texture(tex_data, nx, ny));
//	//list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
//	texture *pertext = new noise_texture(0.25);
//	list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertext));
//	//int ns = 1000;
//	//for (int j = 0; j < ns; j++) {
//	//	boxlist2[j] = new sphere(vec3(165 * get_random(), 165 * get_random(), 165 * get_random()), 10, white);
//	//}
//	//list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));
//	vec3 lookfrom(220, 280, 0);
//	vec3 lookat(220, 280, 300);
//	float dist_to_focus = 10.0;
//	float aperture = 0.0;
//	float vfov = 40.0;
//	cam = camera(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);
//	return new hitable_list(list, l);
//}

image_texture* load_image_texture(const char* mypath) {
	int nx, ny, nr;
	unsigned char* data = stbi_load(mypath, &nx, &ny, &nr, 0);
	return new image_texture(data, nx, ny);
}

int main() {
	int nx = 500;
	int ny = 500;
	int ns = 100;
	srand((unsigned)time(NULL));

	std::ofstream fout("C:\\Users\\HJW\\Desktop\\test.ppm", std::ios::trunc);
	fout << "P3\n" << nx << " " << ny << "\n255\n";
	std::cout << "Shading..\n";

	camera cam;
	std::shared_ptr<hitable> light_shape = std::make_shared<xz_rect>(213, 343, 227, 332, 554, nullptr);
	hitable* world = cornellbox(nx, ny, cam);

	//shade
	for (int j = ny - 1; j >= 0; j--) {
		std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < nx; i++) {
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++) {
				float u = float(i + get_random()) / float(nx);
				float v = float(j + get_random()) / float(ny);
				ray r = cam.get_ray(u, v);
				col += color(r, world, light_shape, 0);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);

			fout << ir << " " << ig << " " << ib << "\n";
		}
	}
	std::cout << "Done.\n";
	return 0;
}