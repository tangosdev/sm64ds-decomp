//cpp
// @symbol _ZN9UkikiCage6RenderEv
/* UkikiCage::Render -- vtable slot 9, ov030 0x02111350. Defined here as a real
 * UkikiCage::Render() method: refresh the model and collision transforms, then
 * draw the Model sub-object at +0xd4 through its vtable (slot 5). The Model is
 * not yet a named member, so that last call keeps the local shadow-vtable
 * spelling; the two transform updates stay as their inherited dBgActor_c
 * symbols. */
#include "UkikiCage.h"
struct Obj { virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3(); virtual void f4(); virtual void m(int); };
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);

int UkikiCage::Render() {
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
    ((Obj *)((char *)this + 0xd4))->m(0);
    return 1;
}
