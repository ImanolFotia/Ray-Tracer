#pragma once
#include <glm/glm.hpp>

class Plane
{
public:
    Plane(float w, float h) : m_Width(w), m_Height(h) {}

    float m_Width, m_Height;
    glm::vec3 m_Normal = glm::vec3(0,1,0); //placeholder

    bool intersect(glm::vec3 l0, glm::vec3 l1, float& t )
    {
        // assuming vectors are all normalized
        float denom = glm::dot(m_Normal, l0);
        if (denom > 1e-6) {
            glm::vec3 p0l0 = glm::vec3(l0.x,0,l0.y) - l1;
            t = glm::dot(p0l0, m_Normal) / denom;
            return (t >= 0);
        }

        return false;
    }
private:

protected:

};

