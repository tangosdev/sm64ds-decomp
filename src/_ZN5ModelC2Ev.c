//cpp
// @symbol _ZN5ModelC2Ev
#include "Model.h"

extern "C" Matrix4x3 IDENTITY_MATRIX4X3;

/* The copy has to go through a struct whose only member is an array. Model.h
 * reaches math/Matrix.h, where Matrix4x3 is spelled `Matrix3x3 r; Vector3 t;`
 * rather than flat -- and a C++ member-wise copy of that scalarises the Vector3
 * tail into four ldr/str pairs where the ROM issues a third ldm/stm. */
struct Matrix4x3Words { s32 w[12]; };

Model::Model()
{
    transformsBuf = 0;
    *(Matrix4x3Words *)&mat4x3 = *(Matrix4x3Words *)&IDENTITY_MATRIX4X3;
}
