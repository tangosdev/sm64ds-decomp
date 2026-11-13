#ifndef SM64DS_MATH_CALCLIB_H
#define SM64DS_MATH_CALCLIB_H
#ifndef __cplusplus
#define Vec3_Dist _Z9Vec3_DistPK7Vector3S1_
#define Vec3_HorzAngle _Z14Vec3_HorzAnglePK7Vector3S1_
#define Vec3_HorzDist _Z13Vec3_HorzDistPK7Vector3S1_
#define Vec3_HorzLen _Z12Vec3_HorzLenPK7Vector3
#endif

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


#endif

extern "C" /* Decrement *p unless it is already 0; return the new value. */
unsigned short DecIfAbove0_Short(unsigned short *);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char *);

/* Step *cur a 1/divisor share toward target; return what is left.
   C linkage: d_camera.cpp's call sites pass the target as int, and a short
   parameter there no longer reproduces (ApproachLookAtAndAngles, func_02009e70). */
extern "C" int ApproachAngle(short *, short, int, int, int);

extern "C" /* Advance the LCG seed and return it. */
int RandomIntInternal(int *);

extern "C" /* Shift a vector right per axis. Reads all three components before writing
   any, so d == s is allowed. */
void Vec3_Asr(Vector3 *, Vector3 *, int);
extern "C" /* ...and left; d may alias s. */
void Vec3_Lsl(Vector3 *, const Vector3 *, int);

extern "C" /* NitroSDK fx_vec (0x02053274..0x020538b8). */
/* out = a + b and a - b, per axis; out may alias either operand. */
void VEC_Add(Vector3 *, Vector3 *, Vector3 *);
extern "C" void VEC_Subtract(Vector3 *, Vector3 *, Vector3 *);
extern "C" /* Length of v. */
int VEC_Mag(const Vector3 *);
extern "C" /* a . b, in fx32. */
Fix12i VEC_DotProduct(const Vector3 *, const Vector3 *);

short Vec3_HorzAngle(const Vector3 *, const Vector3 *);
Fix12i Vec3_HorzLen(const Vector3 *);
int Vec3_Dist(const Vector3 *, const Vector3 *);
extern "C" int Vec3_Equal(const Vector3 *, const Vector3 *);
int Vec3_HorzDist(const Vector3 *, const Vector3 *);

#endif
