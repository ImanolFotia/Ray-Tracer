#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED
#include <vec3.h>

class Ray{
public:
    Ray(vec3 ro, vec3 rd) : m_From(ro), m_To(rd){}

    vec3 m_From;
    vec3 m_To;
};

#endif // RAY_H_INCLUDED
