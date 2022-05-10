#ifndef RAY_H
#define RAY_H

#include "vector.h"

class Ray
{
public:
    Ray() {}
    Ray(const point3 &origin, const vec3 direction) : origin(origin), direction(direction)
    {

    }
    inline point3 at(float t) const
    {
        return origin + (direction * t);
    }
    point3 origin;
    vec3 direction;
private:
};

#endif