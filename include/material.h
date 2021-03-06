#ifndef MATERIAL_H
#define MATERIAL_H

#include "pch.h"

#include "ray.h"
#include "shape.h"
#include "utils.h"

class Material
{
public:
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, col3 &attenuation, Ray& r_out, ThreadLocal& tl) const = 0;
    virtual col3 emitted(float u, float v, point3 &p) const 
    {
        return col3(0, 0, 0);
    }
};

class MaterialList
{
public:
    MaterialList() = default;
    ~MaterialList()
    {
        for (auto mat: materials)
        {
            delete mat;
        }
    }
    template <typename T, typename... Args>
    T* add(Args&&... args)
    {
        T* p_mat = new T(std::forward<Args>(args)...);
        materials.push_back(p_mat);
        return p_mat;
    }

private:
    std::vector<Material*> materials;
};

class Lambertian : public Material
{
public:
    Lambertian(col3 &albedo) : albedo(albedo) {}
    Lambertian(col3 &&albedo) : albedo(albedo) {}

    bool scatter(const Ray &r_in, const HitRecord &rec, col3 &attenuation, Ray& r_out, ThreadLocal& tl) const override
    {
        vec3 scatterDir = rec.normal + tl.randUnitVector();

        if (nearZero(scatterDir))
        {
            scatterDir = rec.normal;
        }

        r_out = Ray(rec.p, scatterDir);
        attenuation = albedo;
        return true;
    }
    col3 albedo;
};

class Metal : public Material
{
public:
    Metal(col3 &albedo, float fuzz) : albedo(albedo), fuzz(fuzz) {}
    Metal(col3 &&albedo, float fuzz) : albedo(albedo), fuzz(fuzz) {}

    bool scatter(const Ray &r_in, const HitRecord &rec, col3 &attenuation, Ray& r_out, ThreadLocal& tl) const override
    {
        vec3 reflected = glm::reflect(glm::normalize(r_in.direction), rec.normal);
        r_out = Ray(rec.p, reflected + fuzz * tl.randInUnitSphereVec3());
        attenuation = albedo;
        return (glm::dot(r_out.direction, rec.normal) > 0);
    }
    col3 albedo;
    float fuzz;
};

class Dielectric : public Material
{
public:
    Dielectric(float ir) : ir(ir) {}

    bool scatter(const Ray &r_in, const HitRecord &rec, col3 &attenuation, Ray& r_out, ThreadLocal& tl) const override
    {
        attenuation = col3(1, 1, 1);
        float refractionRatio = rec.frontFace ? (1.0 / ir) : ir;

        vec3 unitDir = glm::normalize(r_in.direction);

        float cosTheta = fmin(glm::dot(-unitDir, rec.normal), 1.0);
        float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = refractionRatio * sinTheta > 1.0;
        vec3 dir;

        if (cannotRefract || reflectance(cosTheta, refractionRatio) > tl.randFloat())
        {
            dir = glm::reflect(unitDir, rec.normal);
        }
        else
        {
            dir = glm::refract(unitDir, rec.normal, refractionRatio);
        }

        r_out = Ray(rec.p, dir);
        return true;
    }

    float ir;

    private:
        static float reflectance(float cosine, float ref_idx)
        {
            float r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * glm::pow((1 - cosine), 5);
        }
};

class Diffuse : public Material
{
public:
    Diffuse(col3 c) : c(c) {}
    bool scatter(const Ray &r_in, const HitRecord &rec, col3 &attenuation, Ray& r_out, ThreadLocal& tl) const override
    {
        return false;
    }
    col3 emitted(float u, float v, point3 &p) const override
    {
        return c;
    }
    col3 c;

private:

};

#endif
