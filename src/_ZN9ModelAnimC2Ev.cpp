//cpp
// @symbol _ZN9ModelAnimC2Ev
/* Recovered base-object constructor. The compiler emits the Model base step,
 * Animation member construction, both ModelAnim vptr stores, and the body's
 * file initialization. objisolate keeps C2 here.
 */
#include "ModelAnim.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT C2 NAME, AND MSVC NEVER EMITS IT. MSVC
 * makes one constructor per class, ??0ModelAnim, which the class's C1 file
 * already defines, so compiling the definition below as well would define
 * it twice. The base-object variant the ROM spells _ZN9ModelAnimC2Ev is the
 * same body (ModelAnim has no virtual base), and host callers that build a
 * ModelAnim base subobject call it by that flat name. This arm is that name:
 * the one host constructor, run on the object in place. Nothing here
 * reaches mwccarm: it builds the `#else` arm and emits the ROM bytes it
 * always emitted, and the object is byte-identical either way. */
#include <new>
extern "C" ModelAnim *_ZN9ModelAnimC2Ev(ModelAnim *thiz)
{
    ::new ((void *)thiz) ModelAnim();  /* the one host constructor, in place */
    return thiz;
}
#else
ModelAnim::ModelAnim()
{
    file = 0;
}
#endif
