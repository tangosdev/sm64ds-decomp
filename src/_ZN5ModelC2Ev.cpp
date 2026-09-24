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

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT C2 NAME, AND MSVC NEVER EMITS IT. MSVC
 * makes one constructor per class, ??0Model, which the class's C1 file
 * already defines, so compiling the definition below as well would define
 * it twice. The base-object variant the ROM spells _ZN5ModelC2Ev is the
 * same body (Model has no virtual base), and host callers that build a
 * Model base subobject call it by that flat name. This arm is that name:
 * the one host constructor, run on the object in place. Nothing here
 * reaches mwccarm: it builds the `#else` arm and emits the ROM bytes it
 * always emitted, and the object is byte-identical either way. */
#include <new>
extern "C" Model *_ZN5ModelC2Ev(Model *thiz)
{
    ::new ((void *)thiz) Model();  /* the one host constructor, in place */
    return thiz;
}
#else
Model::Model() : transformsBuf(0)
{
    mat4x3 = IDENTITY_MATRIX4X3;
}
#endif
