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
    float u, v;
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
    virtual bool rayHit(const Ray& r, double t_min, double t_max, HitRecord& rec) = 0;
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
    template <typename T, typename... Args>
    void add(Args&&... args)
    {
        shapes.push_back(new T(std::forward<Args>(args)...));
    }
    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec)
    {
        HitRecord tempRec;
        bool hitAnything = false;
        auto closestSoFar = t_max;

        for (auto shape: shapes)
        {
            if (shape->rayHit(r, t_min, closestSoFar, tempRec))
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
    bool rayHit(const Ray& r, double t_min, double t_max, HitRecord& rec) override
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

class NaiveTriangle : public Shape
{
public:
    NaiveTriangle(point3 v0, point3 v1, point3 v2, Material *material) : v0(v0), v1(v1), v2(v2), material(material)
    {
        normal = glm::cross(v1 - v0, v2 - v0);
    }

    bool rayHit(const Ray& r, double t_min, double t_max, HitRecord& rec) override
    {
        float thit, t, u, v;

        vec3 v0v1 = v1 - v0;
        vec3 v0v2 = v2 - v0;
        
        vec3 pvec = glm::cross(r.direction, v0v2);
        
        float det = glm::dot(pvec, v0v1);
        float kEpsilon = 0.00001;

        if (det < kEpsilon) return false;

        float invDet = 1 / det;
        
        vec3 tvec = r.origin - v0;
        u = glm::dot(tvec, pvec) * invDet;
        
        if (u < 0 || u > 1) return false;

        vec3 qvec = glm::cross(tvec, v0v1);
        v = glm::dot(r.direction, qvec) * invDet;
        if (v < 0 || u + v > 1) return false;

        t = glm::dot(v0v2, qvec) * invDet;

        if (t < 0) return false;// std::cout << t << std::endl;

        rec.p = r.at(t);

        rec.t = t;
        rec.normal = normal;
        rec.material = material;
	
        return true;
    }


private:
    point3 v0, v1, v2;
    vec3 normal;
    Material *material;
};


#endif