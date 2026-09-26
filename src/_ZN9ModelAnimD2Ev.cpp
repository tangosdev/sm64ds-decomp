//cpp
// @symbol _ZN9ModelAnimD2Ev
/* Recovered base-object destructor. The empty C++ body makes CodeWarrior
 * restore both vptrs, destroy the Animation member, and destroy Model.
 * objisolate keeps D2 here.
 */
#include "ModelAnim.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D2 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~ModelAnim() it emits,
 * which the class's D1 file already defines, so compiling the definition
 * below as well would define it twice. The base-object variant the ROM
 * spells _ZN9ModelAnimD2Ev is the same body (ModelAnim has no virtual base), and
 * host callers tearing down a ModelAnim base subobject call it by that flat
 * name. This arm is that name: the one host destructor, called qualified
 * so it is a direct call. Nothing here reaches mwccarm: it builds the
 * `#else` arm and emits the ROM bytes it always emitted, and the object is
 * byte-identical either way. */
extern "C" ModelAnim *_ZN9ModelAnimD2Ev(ModelAnim *thiz)
{
    thiz->ModelAnim::~ModelAnim();  /* the D1 body, through the one host symbol */
    return thiz;
}
#else
ModelAnim::~ModelAnim()
{
}
#endif
