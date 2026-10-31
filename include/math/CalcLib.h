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
 * (UpdateAngle, ApproachLinear) are in math/Approach.h. */
#include "math/Approach.h"

extern "C" {

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

/* out = a + b and a - b, per axis; out may alias either operand. */
void AddVec3(Vector3 *, Vector3 *, Vector3 *);
void SubVec3(Vector3 *, Vector3 *, Vector3 *);

short Vec3_HorzAngle(const Vector3 *, const Vector3 *);
Fix12i Vec3_HorzLen(const Vector3 *);
int Vec3_Dist(const Vector3 *, const Vector3 *);
int Vec3_Equal(const Vector3 *, const Vector3 *);
int Vec3_HorzDist(const Vector3 *, const Vector3 *);
}

#endif
