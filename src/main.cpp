/*The MIT License (MIT)

Copyright (c) 2021-Present, Wencong Yang (yangwc3@mail2.sysu.edu.cn).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#include <array>
#include <vector>
#include <thread>
#include <iostream>

#include "WindowsApp.h"
#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "triangle.h"

// 修改这个参数以修改渲染精度
#define SAMPLE_PIXEL (1000)

static std::vector<std::vector<color>> gCanvas;		//Canvas

// The width and height of the screen
const auto aspect_ratio = 1.0;
const int gWidth = 600;
const int gHeight = static_cast<int>(gWidth / aspect_ratio);

void rendering();

int main(int argc, char* args[])
{
	// Create window app handle
	WindowsApp::ptr winApp = WindowsApp::getInstance(gWidth, gHeight, "CGAssignment4: Ray Tracing");
	if (winApp == nullptr)
	{
		std::cerr << "Error: failed to create a window handler" << std::endl;
		return -1;
	}

	// Memory allocation for canvas
	gCanvas.resize(gHeight, std::vector<color>(gWidth));

	// Launch the rendering thread
	// Note: we run the rendering task in another thread to avoid GUI blocking
	std::thread renderingThread(rendering);

	// Window app loop
	while (!winApp->shouldWindowClose())
	{
		// Process event
		winApp->processEvent();

		// Display to the screen
		winApp->updateScreenSurface(gCanvas);

	}

	renderingThread.join();

	return 0;
}


hittable_list random_scene() {
	hittable_list world;
	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b +
				0.9 * random_double());
			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;
				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2,
						sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2,
						sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2,
						sphere_material));
				}
			}
		}
	}
	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));
	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));
	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
	return world;
}

void rendering()
{
	double startFrame = clock();

	printf("CGAssignment4 (built %s at %s) \n", __DATE__, __TIME__);
	std::cout << "Ray-tracing based rendering launched..." << std::endl;

	hittable_list world;

	auto red = make_shared<lambertian>(color(.65, .05, .05));
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	auto green = make_shared<lambertian>(color(.12, .45, .15));
	auto blue = make_shared<lambertian>(color(.12, .18, .30));
	auto light = make_shared<diffuse_light>(color(15, 15, 15));
	//auto white = make_shared<diffuse_light>(color(1, 1, 1));
	auto earth = make_shared<lambertian>(make_shared<image_texture>("../image/earthmap.jpg"));
	auto football = make_shared<lambertian>(make_shared<image_texture>("../image/football.jpg"));
	auto golf = make_shared<lambertian>(make_shared<image_texture>("../image/golf.jpg"));
	auto t1 = make_shared<lambertian>(make_shared<image_texture>("../image/texture1.jpg"));

	// Cornell box sides
	world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 0, 555), vec3(0, 555, 0), green));
	world.add(make_shared<quad>(point3(0, 0, 555), vec3(0, 0, -555), vec3(0, 555, 0), red));
	world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
	world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 0, -555), white));
	world.add(make_shared<quad>(point3(555, 0, 555), vec3(-555, 0, 0), vec3(0, 555, 0), white));

	// Light
	world.add(make_shared<quad>(point3(213, 554, 227), vec3(130, 0, 0), vec3(0, 0, 105), light)); 
	//world.add(make_shared<sphere>(point3(200, 300, 220), 40, light));

	// Box
	shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	world.add(box1);
	
	//triangle
	point3 v0(0, 0, 100);
	point3 v1(0, 100, 0);
	point3 v2(100,0,0);
	// 创建三角形对象，传入三个顶点和材质
	auto light_triangle = make_shared<triangle>(v0, v1, v2, light);
	// 将三角形添加到场景中
	//world.add(light_triangle);

	//obj file
	world = triangle::load_obj_model("../obj/lifangti14.obj", white,world);

	//Sphere
	auto glass = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(190, 90, 190), 90, t1));
	world.add(make_shared<sphere>(point3(450, 70, 160), 70, football));
	world.add(make_shared<sphere>(point3(330, 50, 90), 50, blue));

	// Light Sources
	hittable_list lights;
	auto m = shared_ptr<material>();
	lights.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), m));

	camera cam;

	cam.aspect_ratio = 1.0;
	cam.image_width = 600;
	cam.samples_per_pixel = SAMPLE_PIXEL; 
	cam.max_depth = 10;
	cam.background = color(0, 0, 0);

	cam.vfov = 40.0;
	cam.lookfrom = point3(278, 278, -800);
	cam.lookat = point3(278, 278, 0);
	cam.vup = vec3(0, -1, 0);

	cam.defocus_angle = 0.0;

	cam.render(world, lights);
	//cam.muti_thread_render(world, lights);

	double endFrame = clock();
	double timeConsuming = static_cast<double>(endFrame - startFrame) / CLOCKS_PER_SEC;
	std::cout << "Ray-tracing based rendering over..." << std::endl;
	std::cout << "The rendering task took " << timeConsuming << " seconds" << std::endl;
}