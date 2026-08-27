//cpp
// @symbol _ZN5ModelC2Ev
/* Recovered base-object constructor.
 *
 * This is the same source-level constructor as _ZN5ModelC1Ev. CodeWarrior
 * emits both ABI variants from this definition; objisolate keeps C2 here.
 */
#include "common.h"
#include "Model.h"

extern Matrix4x3 IDENTITY_MATRIX4X3;

Model::Model() : transformsBuf(0)
{
    mat4x3 = IDENTITY_MATRIX4X3;
}
