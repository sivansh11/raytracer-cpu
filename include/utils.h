#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <vector.h>
#include <stdlib.h>
#include <random>


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


struct ThreadLocal
{
    std::mt19937 randomNumberGenerator;
    std::uniform_real_distribution<float> distribution {0.f, 1.f};

    void init(uint32_t seed)
    {
        randomNumberGenerator.seed(seed);
    }

    float randFloat()
    {
        return distribution(randomNumberGenerator);
    }

    inline float randFloat(float min, float max)
    {
        return min + (max-min)*randFloat();
    }

    vec3 randVec3()
    {
        return vec3 {randFloat(), randFloat(), randFloat()};
    }
    vec3 randVec3(float min, float max)
    {
        return vec3 {randFloat(min, max), randFloat(min, max), randFloat(min, max)};
    }
    vec3 randInUnitSphereVec3()
    {
        while (true) {
            auto p = randVec3(-1,1);
            if (glm::length2(p) >= 1) continue;
            return p;
        }
    }
    vec3 randUnitVector()
    {
        return glm::normalize(randInUnitSphereVec3());
    }

    vec3 randInHemisphere(vec3 &normal)
    {
        vec3 inUnitSphere = randInUnitSphereVec3();
        if (glm::dot(inUnitSphere, normal) > 0.0)
        {
            return inUnitSphere;
        }
        return -inUnitSphere;
    }
};

std::ostream& operator << (std::ostream &out, glm::vec3 &vec);
std::ostream& operator << (std::ostream &out, glm::vec3 &&vec);

float clamp(float x, float min, float max);

/*
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
*/
bool nearZero(vec3 &vec);
void colorConversion(uint32_t col, uint8_t *newCol);
#endif
