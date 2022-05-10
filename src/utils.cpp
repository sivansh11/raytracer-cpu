#include "utils.h"

#include "vector.h"

std::string readFileContents(const char *filePath)
{
    std::string contents, lineBuff;
    std::ifstream file(filePath);
    if (file.is_open())
    {
        while (getline(file, lineBuff))
        {
            contents += lineBuff + '\n';
        }
        contents.pop_back();
        file.close();
    }
    else std::__throw_runtime_error("Couldnt open file given!");
    return contents;
}

float clamp(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

uint32_t color(float p_r, float p_g, float p_b, float p_a)
{
    uint8_t r = clamp(p_r, 0, 0.999) * 256;
    uint8_t g = clamp(p_g, 0, 0.999) * 256;
    uint8_t b = clamp(p_b, 0, 0.999) * 256;
    uint8_t a = clamp(p_a, 0, 0.999) * 256;
    uint32_t res;
    res = r | g << 8 | b << 16 | a << 24;
    return res;
}

uint32_t color(col3 &col)
{
    return color(col.r, col.g, col.b, 1);
}


std::ostream& operator << (std::ostream &out, glm::vec3 &vec)
{
    return out << vec.x << ' ' << vec.y << ' ' << vec.z;
}
std::ostream& operator << (std::ostream &out, glm::vec3 &&vec)
{ return out << vec; }

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

bool nearZero(vec3 &vec)
{
    constexpr float s = 1e-8;
    return (fabs(vec.x) < s) && (fabs(vec.y) < s) && (fabs(vec.z) < s);
}
