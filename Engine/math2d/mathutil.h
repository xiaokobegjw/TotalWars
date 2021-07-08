#pragma once

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cassert>

#ifdef _WIN32
#include <stdint.h>
#endif

extern const double    MAX_DOUBLE;
extern const double    MIN_DOUBLE;
extern const double    MAX_FLOAT;
extern const double    MIN_FLOAT;

extern const int32_t   MIN_INT32;
extern const int32_t   MAX_INT32;
extern const int16_t   MIN_INT16;
extern const int16_t   MAX_INT16;
extern const int8_t    MIN_INT8;
extern const int8_t    MAX_INT8;
extern const uint32_t  MIN_UINT32;
extern const uint32_t  MAX_UINT32;
extern const uint16_t  MIN_UINT16;
extern const uint16_t  MAX_UINT16;
extern const uint8_t   MIN_UINT8;
extern const uint8_t   MAX_UINT8;

template <typename Real>
class Math
{
public:
    // Wrappers to hide implementations of functions.  The ACos and ASin
    // functions clamp the input argument to [-1,1] to avoid NaN issues
    // when the input is slightly larger than 1 or slightly smaller than -1.
    // Other functions have the potential for using a fast and approximate
    // algorithm rather than calling the standard math library functions.
    static Real ACos (Real value);
    static Real ASin (Real value);
    static Real ATan (Real value);
    static Real ATan2 (Real y, Real x);
    static Real Ceil (Real value);
    static Real Cos (Real value);
    static Real Exp (Real value);
    static Real FAbs (Real value);
    static Real Floor (Real value);
    static Real FMod (Real x, Real y);
    static Real InvSqrt (Real value);
    static Real Log (Real value);
    static Real Log2 (Real value);
    static Real Log10 (Real value);
    static Real Pow (Real base, Real exponent);
    static Real Sin (Real value);
    static Real Sqr (Real value);
    static Real Sqrt (Real value);
    static Real Tan (Real value);
    static Real Sign(Real value);
    static Real Clamp(Real value, Real minValue, Real maxValue);
    
    static bool isEqual(Real a, Real b);
    
    
    // Fast evaluation of trigonometric and inverse trigonometric functions
    // using polynomial approximations.  The speed ups vary with processor.

    // The input must be in [0,pi/2].
    static Real FastSin0 (Real angle);
    static Real FastSin1 (Real angle);

    // The input must be in [0,pi/2]
    static Real FastCos0 (Real angle);
    static Real FastCos1 (Real angle);

    // The input must be in [0,pi/4].
    static Real FastTan0 (Real angle);
    static Real FastTan1 (Real angle);

    // The input must be in [0,1].
    static Real FastInvSin0 (Real value);
    static Real FastInvSin1 (Real value);

    // The input must be in [0,1].
    static Real FastInvCos0 (Real value);
    static Real FastInvCos1 (Real value);

    // The input must be in [-1,1]. 
    static Real FastInvTan0 (Real value);
    static Real FastInvTan1 (Real value);

    // Fast approximations to exp(-x).  The input x must be in [0,infinity).
    static Real FastNegExp0 (Real value);
    static Real FastNegExp1 (Real value);
    static Real FastNegExp2 (Real value);
    static Real FastNegExp3 (Real value);
    
    static Real DegToRad(Real Degrees);
    static Real RadToDeg(Real Radians);
    
    // Common constants.
    static const Real EPSILON;
    static const Real ZERO_TOLERANCE;
    static const Real MAX_REAL;
    static const Real PI;
    static const Real TWO_PI;
    static const Real HALF_PI;
    static const Real INV_PI;
    static const Real INV_TWO_PI;
    static const Real DEG_TO_RAD;
    static const Real RAD_TO_DEG;
    static const Real LN_2;
    static const Real LN_10;
    static const Real INV_LN_2;
    static const Real INV_LN_10;
    static const Real SQRT_2;
    static const Real INV_SQRT_2;
    static const Real SQRT_3;
    static const Real INV_SQRT_3;
};

#include "mathutil.inl"

typedef Math<float> Mathf;
typedef Math<double> Mathd;
