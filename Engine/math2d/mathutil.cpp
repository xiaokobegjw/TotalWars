#include "mathutil.h"
#include <limits>

const double    MAX_DOUBLE = std::numeric_limits<double>::max();
const double    MIN_DOUBLE = std::numeric_limits<double>::min();
const double    MAX_FLOAT = std::numeric_limits<float>::max();
const double    MIN_FLOAT = std::numeric_limits<float>::min();

const int32_t   MIN_INT32 = (int32_t)0xffffffff; // (-2147483646);
const int32_t   MAX_INT32 = (int32_t)0x7fffffff; // ( 2147483647);
const int16_t   MIN_INT16 = (int16_t)0xffff; // (-32768);
const int16_t   MAX_INT16 = (int16_t)0x7fff; // ( 32767);
const int8_t    MIN_INT8 = (int8_t)0xff; // (-128);
const int8_t    MAX_INT8 = (int8_t)0x7f; // ( 127);
const uint32_t  MIN_UINT32 = (uint32_t)0;
const uint32_t  MAX_UINT32 = (uint32_t)0xffffffff;
const uint16_t  MIN_UINT16 = (uint16_t)0;
const uint16_t  MAX_UINT16 = (uint16_t)0xffff;
const uint8_t   MIN_UINT8 = (uint8_t)0;
const uint8_t   MAX_UINT8 = (uint8_t)0xff;

template<> const float Math<float>::EPSILON = FLT_EPSILON;
template<> const float Math<float>::ZERO_TOLERANCE = 1e-06f;
template<> const float Math<float>::MAX_REAL = FLT_MAX;
template<> const float Math<float>::PI = (float)(4.0*atan(1.0));
template<> const float Math<float>::TWO_PI = 2.0f*Math<float>::PI;
template<> const float Math<float>::HALF_PI = 0.5f*Math<float>::PI;
template<> const float Math<float>::INV_PI = 1.0f/Math<float>::PI;
template<> const float Math<float>::INV_TWO_PI = 1.0f/Math<float>::TWO_PI;
template<> const float Math<float>::DEG_TO_RAD = Math<float>::PI/180.0f;
template<> const float Math<float>::RAD_TO_DEG = 180.0f/Math<float>::PI;
template<> const float Math<float>::LN_2 = Math<float>::Log(2.0f);
template<> const float Math<float>::LN_10 = Math<float>::Log(10.0f);
template<> const float Math<float>::INV_LN_2 = 1.0f/Math<float>::LN_2;
template<> const float Math<float>::INV_LN_10 = 1.0f/Math<float>::LN_10;
template<> const float Math<float>::SQRT_2 = (float)(sqrt(2.0));
template<> const float Math<float>::INV_SQRT_2 = 1.0f/Math<float>::SQRT_2;
template<> const float Math<float>::SQRT_3 = (float)(sqrt(3.0));
template<> const float Math<float>::INV_SQRT_3 = 1.0f/Math<float>::SQRT_3;

template<> const double Math<double>::EPSILON = DBL_EPSILON;
template<> const double Math<double>::ZERO_TOLERANCE = 1e-08;
template<> const double Math<double>::MAX_REAL = DBL_MAX;
template<> const double Math<double>::PI = 4.0*atan(1.0);
template<> const double Math<double>::TWO_PI = 2.0*Math<double>::PI;
template<> const double Math<double>::HALF_PI = 0.5*Math<double>::PI;
template<> const double Math<double>::INV_PI = 1.0/Math<double>::PI;
template<> const double Math<double>::INV_TWO_PI = 1.0/Math<double>::TWO_PI;
template<> const double Math<double>::DEG_TO_RAD = Math<double>::PI/180.0;
template<> const double Math<double>::RAD_TO_DEG = 180.0/Math<double>::PI;
template<> const double Math<double>::LN_2 = Math<double>::Log(2.0);
template<> const double Math<double>::LN_10 = Math<double>::Log(10.0);
template<> const double Math<double>::INV_LN_2 = 1.0/Math<double>::LN_2;
template<> const double Math<double>::INV_LN_10 = 1.0/Math<double>::LN_10;
template<> const double Math<double>::SQRT_2 = sqrt(2.0);
template<> const double Math<double>::INV_SQRT_2 = 1.0f/Math<float>::SQRT_2;
template<> const double Math<double>::SQRT_3 = sqrt(3.0);
template<> const double Math<double>::INV_SQRT_3 = 1.0f/Math<float>::SQRT_3;
