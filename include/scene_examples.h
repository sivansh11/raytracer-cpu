#ifndef EXAMPLES_H
#define EXAMPLES_H

#include "shape.h"
#include "material.h"
#include "setting.h"

void my_example_scene(ShapeList &world, MaterialList &materials, Settings &setting, point3 &from, point3 &at)
{
    from = point3(10, 5, -2);
    at = point3(0, 0, -1);
    Material *ground = materials.add<Lambertian>(col3(.8, .8, .2));
    Material *center = materials.add<Diffuse>(col3(1, 1, 1));
    Material *mat1 = materials.add<Lambertian>(col3(0, 0, 1));
    Material *mat2 = materials.add<Dielectric>(1.5);
    world.add<Sphere>(point3(0, -100.5, -1), 100, ground);
    world.add<Sphere>(point3(0, 0, -1), 0.5, mat2);
    world.add<Sphere>(point3(0, 0, 0), 0.5, mat1);
    world.add<Sphere>(point3(0, 0, -2), 0.5, center);
    setting.background = col3{0};
}

void triangle_example(ShapeList &world, MaterialList &materials, Settings &setting, point3 &from, point3 &at)
{
    from = point3(0);
    at = point3(0, 0, 1);
    Material *mat = materials.add<Lambertian>(col3(1, 0, 0));
    world.add<NaiveTriangle>(point3(1, -.5, 2), point3(-1, -.5, 2), point3(0, 1, 2), mat);
    setting.background = col3{1};
    setting.maxDepth = 25;
    setting.samplesPerPixel = 25;
    setting.fov = 90;
}


#endif