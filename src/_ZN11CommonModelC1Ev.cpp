//cpp
// @symbol _ZN11CommonModelC1Ev
/* recovered: real C++ constructor
 *
 * Only the member initialization is written by hand: `data(0)` is the zero at
 * +0x8 and the body's one assignment is the mat4x3 copy from IDENTITY_MATRIX4X3.
 * The base call and vptr store around them are what `CommonModel : ModelBase`
 * with a declared-not-defined base constructor already means.
 *
 * common.h comes first so this TU sees the flat Matrix4x3 { s32 m[12]; }
 * spelling (include/common.h): under the structured spelling the same copy
 * scalarizes and lands +28 bytes long, where the ROM block-moves all twelve
 * words. Same recipe as _ZN5ModelC1Ev.cpp, which documents it in full.
 */
#include "common.h"
#include "CommonModel.h"

extern Matrix4x3 IDENTITY_MATRIX4X3;

CommonModel::CommonModel() : data(0)
{
    mat4x3 = IDENTITY_MATRIX4X3;
}
