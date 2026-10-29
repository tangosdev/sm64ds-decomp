#ifndef SM64DS_MATH_APPROACH_H
#define SM64DS_MATH_APPROACH_H

/* The C++ steppers in src/ApproachAngle.cpp (0x0203adec..0x0203b000), and
 * the Vector3 one beside Vec3_ApproachHorz: move a
 * value toward a target by a bounded step each frame. EAD's GameCube code
 * keeps the same pair as cLib_chaseS / cLib_addCalcAngleS2.
 *
 * Kept apart from math/CalcLib.h so a caller takes these without the Vector3
 * helpers, whose local `const void *` spellings many actors still carry.
 * Parameters stay unnamed: a name here takes an @NNN in every includer. */

/* Turn angle toward target by 1/divisor of the gap, at most maxStep. */
void UpdateAngle(short &, short, int, short);

/* Step value toward target by at most step; nonzero once it arrives. */
int ApproachLinear(int &, int, int);
bool ApproachLinear(short &, short, short);

/* The same with a 32-bit product; the int & overload steps only the low byte. */
int ApproachLinear2(int &, int, int);
int ApproachLinear2(short &, short, short);

/* Move a point straight toward target by at most step, a Fix12 distance
 * (src/Vec3_ApproachHorz.cpp, 0x0203b89c); nonzero once it arrives. The step
 * is a plain int, as Fix12i is: a by-value Fix12<int> costs every caller a
 * literal-pool reload (notes/mwccarm-codegen.md 6az). */
struct Vector3;
int ApproachLinear(Vector3 &, const Vector3 &, int);

#endif
