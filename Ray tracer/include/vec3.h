#ifndef VEC3_H_INCLUDED
#define VEC3_H_INCLUDED

#include <math.h>
#include <glm/glm.hpp>

struct vec3 {
    vec3(){}
    vec3(float a, float b, float c) : x(a), y(b), z(c) {}
    float x;
    float y;
    float z;

    vec3 operator+(vec3 v) {
        return vec3(x+v.x, y+v.y, z+v.z);
    }
    vec3 operator-(vec3 v) {
        return vec3(x-v.x, y-v.y, z-v.z);
    }
    vec3 operator*(vec3 v) {
        return vec3(x*v.x, y*v.y, z*v.z);
    }
    vec3 operator*(float v) {
        return vec3(x*v, y*v, z*v);
    }
    vec3 operator/(vec3 v) {
        return vec3(x/v.x, y/v.y, z/v.z);
    }
};
#pragma omp declare simd
static float length(vec3 vector3) {
    float l = sqrt((vector3.x * vector3.x) + ( vector3.y * vector3.y ) + ( vector3.z * vector3.z ));

    if(l < 0)
        l *= -1;

    return l;
}
#pragma omp declare simd
static vec3 normalize(vec3 vector3) {
    float l = length(vector3);
    vector3.x = vector3.x/l;
    vector3.y = vector3.y/l;
    vector3.z = vector3.z/l;

    return vector3;
}
#pragma omp declare simd
static float dot(vec3 vec1, vec3 vec2) {
    return ((vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z));
}
#pragma omp declare simd
static float max(float a, float b) {
    return (a > b) ? a : b;
}

#pragma cmp declare simd
static vec3 reflect(vec3 d, vec3 n)
{
    return (n * 2.0f * dot(n, d)) - d;
}

template <typename T = glm::vec3>
vec3 from_glm(T v3){
    return vec3(v3.x, v3.y, v3.z);
}

#endif // VEC3_H_INCLUDED
