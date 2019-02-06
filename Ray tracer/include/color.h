#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED
#include <glm/glm.hpp>
class Color
{

public:
    Color(int x, int y, int z) : r(x), g(y), b(z)
    {}
    Color(glm::vec3 col)
    {
        r = col.x;
        g = col.y;
        b = col.z;
    }
    Color(){r=g=b=0.0;}

    Color(float a){r=g=b=a;}

    virtual ~Color(){}

    float r, g, b;

    Color operator+(double a)
    {
        return Color(this->r + a, this->g + a, this->b + a);
    }
    Color operator+(Color col)
    {
        return Color(this->r + col.r, this->g + col.g, this->b+col.b);
    }

    Color operator*(double a)
    {
        return Color(this->r * a, this->g * a, this->b * a);
    }

    Color operator*(Color col)
    {
        return Color(this->r * col.r, this->g * col.g, this->b*col.b);
    }

    void clamp()
    {
        r = glm::clamp((float)this->r, 0.0f, 255.0f);
        g = glm::clamp((float)this->g, 0.0f, 255.0f);
        b = glm::clamp((float)this->b, 0.0f, 255.0f);
    }


};

#endif // COLOR_H_INCLUDED
