#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "ray.h"
#include "utils.h"

class Camera
{
public:
    Camera(point3 &&from, point3 &&at, vec3 &&up, float aspectRatio, float fov) : aspectRatio(aspectRatio), fov(fov)
    {
        set(from, at, up, aspectRatio, fov);
    }
    Camera(point3 &from, point3 &at, vec3 &up, float aspectRatio, float fov) : aspectRatio(aspectRatio), fov(fov)
    {
        set(from, at, up, aspectRatio, fov);
    }
    void set(point3 from, point3 at, vec3 up, float aspectRatio, float fov)
    {
        float theta = glm::radians(fov);
        float h = glm::tan(theta / 2);
        vpHeight = 2.0f * h;
        vpWidth = aspectRatio * vpHeight;

        vec3 w = glm::normalize(from - at);
        vec3 u = glm::normalize(glm::cross(up, w));
        vec3 v = glm::cross(w, u);

        origin = from;
        horizontal = vpWidth * u;
        vertical = vpHeight * v;
        lowerLeftCorner = origin - (horizontal / 2.f) - (vertical / 2.f) - w;
    }
    Ray getRay(float u, float v)
    {
        return Ray(origin, lowerLeftCorner + (u * horizontal) + (v * vertical) - origin);
    }

private:
    point3 origin;
    point3 lowerLeftCorner;
    vec3 horizontal;
    vec3 vertical;
    float aspectRatio;
    float fov;
    float vpHeight;
    float vpWidth;
};

#endif