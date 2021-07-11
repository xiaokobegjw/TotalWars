#pragma once

#include <random>

/*
 ** return a random integer in the interval [a, b]
 */
inline int random_int(int a, int b) {
    static std::default_random_engine e{std::random_device{}()}; // avoid "Most vexing parse"
    static std::uniform_int_distribution<int> u;
    
    return u(e, decltype(u)::param_type(a, b));
}

/*
 ** return a random real in the interval [a, b] (also [a, b))
 */
inline float random_float(float a, float b) {
    static std::default_random_engine e{std::random_device{}()};
    static std::uniform_real_distribution<float> u;
    
    return u(e, decltype(u)::param_type(a, b));
}

/*
 ** return a random real in the interval [a, b] (also [a, b))
 */
inline double random_double(double a, double b) {
    static std::default_random_engine e{std::random_device{}()};
    static std::uniform_real_distribution<double> u;
    
    return u(e, decltype(u)::param_type(a, b));
}

//returns a random double in the range -1 < n < 1
inline double random_clamped()
{
    return random_double(-1,1);
}

//returns a random bool
inline bool random_bool()
{
    if (random_float(0,1) > 0.5) return true;
    
    else return false;
}

inline float random_float()
{
    return random_float(0,1);
}
