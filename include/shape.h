#ifndef SHAPE_H
#define SHAPE_H

#include "ray.h"
#include "utils.h"

#include <vector>

class Material;

struct HitRecord
{
    point3 p;
    vec3 normal;
    float t;
    bool frontFace;
    Material *material;

    inline void setFaceNormal(const Ray& r, const vec3& outwardNormal)
    {
        frontFace = glm::dot(r.direction, outwardNormal) < 0;
        normal = frontFace ? outwardNormal: -outwardNormal;
    }
};


class Shape
{
public:
    virtual bool rayIntersection(const Ray& r, double t_min, double t_max, HitRecord& rec) = 0;
};

class ShapeList
{
public:
    ShapeList() = default;
    ~ShapeList()
    {
        for (auto shape: shapes)
        {
            delete shape;
        }
    }
    void add(Shape *shape)
    {
        shapes.push_back(shape);
    }
    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec)
    {
        HitRecord tempRec;
        bool hitAnything = false;
        auto closestSoFar = t_max;

        for (auto shape: shapes)
        {
            if (shape->rayIntersection(r, t_min, closestSoFar, tempRec))
            {
                hitAnything = true;
                closestSoFar = tempRec.t;
                rec = tempRec;
            }
        }

        return hitAnything;
    }
    std::vector<Shape*> shapes;
};

class Sphere : public Shape
{
public:
    Sphere(point3 &&c, float r, Material *material) : cen(c), rad(r), material(material) {}
    Sphere(point3 &c, float r, Material *material) : cen(c), rad(r), material(material) {}
    bool rayIntersection(const Ray& r, double t_min, double t_max, HitRecord& rec) override
    {
        vec3 oc = r.origin - cen;
        float a = glm::length2(r.direction);
        float half_b = glm::dot(oc, r.direction);
        float c = glm::length2(oc) - (rad * rad);

        float dis = (half_b * half_b) - (a * c);
        if (dis < 0) return false;

        float sqrtd = glm::sqrt(dis);
        float root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root)
        {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(root);
        vec3 outwardNormal = (rec.p - cen) / rad;
        rec.setFaceNormal(r, outwardNormal);
        rec.material = material;
        return true;

    }
    point3 cen;
    float rad;
    Material *material;
};

#endif