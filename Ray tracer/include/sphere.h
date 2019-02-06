#ifndef SPHERE_H_INCLUDED
#define SPHERE_H_INCLUDED
#include <vec3.h>
#include <ray.h>
class Sphere
{

public:
    Sphere(vec3 c, float r) : m_Center(c), m_Radius(r) {}
    vec3 m_Center;
    float m_Radius;
    float deltaradius;

    void setPosition(vec3 pos)
    {
        m_Center = pos;
    }

    #pragma omp declare simd
    float intersect(vec3 ro, vec3 rd, float& t)
    {
        vec3 oc = ro - m_Center;
        float b = 2.0*dot(oc, rd);
        float c = dot(oc, oc) - m_Radius * m_Radius;

        float disc = b*b - 4.0 * c;

        if(disc < 0.0) return 0;

        float t0 = (-b-sqrt(disc))*0.5;
        float t1 = (-b+sqrt(disc))*0.5;

        t = (t0 < t1) ? t0 : t1;

        return 1;
    }

    #pragma omp declare simd
    float intersect(Ray ray, float& t)
    {
        vec3 oc = ray.m_From - m_Center;
        //float deltaradius = sin(glm::atan(ray.m_From.x, ray.m_From.y)*25)*60 + m_Radius;

        float b = 2.0 * dot(oc, ray.m_To);
        float c = dot(oc, oc) - m_Radius * m_Radius;

        float disc = b*b - 4.0 * c;

        if(disc < 0.0) return 0;

        float t0 = (-b-sqrt(disc))*0.5;
        float t1 = (-b+sqrt(disc))*0.5;

        t = (t0 < t1) ? t0 : t1;

        return 1;
    }


    vec3 getNormal(vec3 i)
    {
        return (i - m_Center);
    }
};

#endif // SPHERE_H_INCLUDED
