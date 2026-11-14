#ifndef SM64DS_MATH_CALCLIB_H
#define SM64DS_MATH_CALCLIB_H

#include "common.h"

/* The arm9 scalar and vector helpers that sit just ahead of the matrix
 * library (see math/MtxStack.h): the short and byte timers at
 * 0x0203adbc..0x0203add4, the linear and angle steppers at
 * 0x0203adec..0x0203b0e8, and the Vector3 distance/angle helpers (with the
 * random seed step at 0x0203b990) at 0x0203b770..0x0203d170. One run of the
 * ROM, most likely one library file, in the place EAD's GameCube code keeps
 * cLib_calcTimer, cLib_chaseS, cLib_addCalcAngleS2 and cLib_targetAngleY.
 *
 * Parameter types follow each DEFINITION in src/. Parameters stay unnamed: a
 * name here takes an @NNN in every includer.
 *
 * Only helpers some includer calls are declared so far. The C++ steppers
 * (UpdateAngle, ApproachLinear) are in math/Approach.h, which C++ includers
 * get with this header; C includers get the C helpers alone. */
#ifdef __cplusplus
#include "math/Approach.h"

extern "C" {
#endif

/* Decrement *p unless it is already 0; return the new value. */
unsigned short DecIfAbove0_Short(unsigned short *);
unsigned char DecIfAbove0_Byte(unsigned char *);

/* Step *cur a 1/divisor share toward target; return what is left. */
int ApproachAngle(short *, short, int, int, int);

/* Advance the LCG seed and return it. */
int RandomIntInternal(int *);

/* Shift a vector right per axis. Reads all three components before writing
   any, so d == s is allowed. */
void Vec3_Asr(Vector3 *, Vector3 *, int);
/* ...and left; d may alias s. */
void Vec3_Lsl(Vector3 *, const Vector3 *, int);

/* NitroSDK fx_vec (0x02053274..0x020538b8). */
/* out = a + b and a - b, per axis; out may alias either operand. */
void VEC_Add(Vector3 *, Vector3 *, Vector3 *);
void VEC_Subtract(Vector3 *, Vector3 *, Vector3 *);
/* Length of v. */
int VEC_Mag(const Vector3 *);
/* a . b, in fx32. */
fx32 VEC_DotProduct(const Vector3 *, const Vector3 *);
/* Cross product and in-place normalizer. */
void VEC_CrossProduct(const Vector3 *, const Vector3 *, Vector3 *);
void VEC_Normalize(const Vector3 *, Vector3 *);
/* The same two add/subtract routines under the name the newer TUs use. */
void Vec3_Add(Vector3 *, const Vector3 *, const Vector3 *);
void Vec3_Sub(Vector3 *, const Vector3 *, const Vector3 *);

short Vec3_HorzAngle(const Vector3 *, const Vector3 *);
fx32 Vec3_HorzLen(const Vector3 *);
int Vec3_Dist(const Vector3 *, const Vector3 *);
int Vec3_Equal(const Vector3 *, const Vector3 *);
int Vec3_HorzDist(const Vector3 *, const Vector3 *);

/* The in-place normalizer: 0 when the vector was too short. */
int NormalizeVec3IfNonZero(Vector3 *);

/* out = in * scale and v *= scale, per axis. */
void Vec3_MulScalar(Vector3 *, const Vector3 *, int);
void Vec3_MulScalarInPlace(Vector3 *, int);

#ifdef __cplusplus
}
#endif

/* C++ linkage: defined in a C++ file and called only from C++. */
short Vec3_VertAngle(const Vector3 *, const Vector3 *);

#endif
