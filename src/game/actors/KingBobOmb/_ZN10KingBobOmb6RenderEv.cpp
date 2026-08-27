//cpp
// @symbol _ZN10KingBobOmb6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "KingBobOmb.h"
extern "C" {
extern int _ZN5Model6RenderEPK7Vector3(void *m, void *v);
}

int KingBobOmb::Render()
{
    void *r1 = (void*)*(int*)((char*)&mHeldActor);
    if (r1 != 0) {
        int r0 = *(int*)((char*)&mFlags);
        int flag = (r0 & 0x4000) ? 1 : 0;
        if (flag != 0) {
            if (*(int*)((char*)r1 + 0xc8) != 0) {
                func_ov078_02125f8c(((void *)this));
            }
        }
    }
    _ZN5Model6RenderEPK7Vector3((char*)((void *)this) + 0x2cc, (void*)0);
    return 1;
}
