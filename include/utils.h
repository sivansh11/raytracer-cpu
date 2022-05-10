#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <vector.h>
#include <stdlib.h>

std::string readFileContents(const char *filePath);

uint32_t color(float p_r, float p_g, float p_b, float p_a);

uint32_t color(col3 &col);

namespace glm
{
    inline float length2(const vec3& vec) 
    {
        float length = glm::length(vec);
        return length * length;
    }
    
} // namespace glm

std::ostream& operator << (std::ostream &out, glm::vec3 &vec);
std::ostream& operator << (std::ostream &out, glm::vec3 &&vec);

float clamp(float x, float min, float max);

inline float randFloat()
{
    return float(rand()) / (RAND_MAX);
}

inline float randFloat(float min, float max)
{
    return min + (max-min)*randFloat();
}

vec3 randVec3();
vec3 randVec3(float min, float max);
vec3 randInUnitSphereVec3();
vec3 randUnitVector();
vec3 randInHemisphere(vec3 &normal);
bool nearZero(vec3 &vec);

#endif