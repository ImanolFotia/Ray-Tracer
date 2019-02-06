#ifndef HELPERS_HPP_INCLUDED
#define HELPERS_HPP_INCLUDED

static float clamp(float a, float min, float max)
{
    if(a > max)
        a = max;
    if(min > a)
        a = min;

    return a;
}

#endif // HELPERS_HPP_INCLUDED
