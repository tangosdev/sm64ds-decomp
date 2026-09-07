//cpp
#include "types.h"
// @symbol _ZN6Number6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Number.h"
extern "C" {
extern void* _ZN8dActor_c15FindWithActorIDEjPS_(u32 id, void* prev);
extern void _ZN7fBase_c18MarkForDestructionEv(void* thiz);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* ts, void* mc);
extern int data_0209b454;
}

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

int Number::Render()
{
    if (data_0209b454 & 0x4000000) {
        void* a = 0;
        u32 id = 0xb2;
        for (;;) {
            if ((a = _ZN8dActor_c15FindWithActorIDEjPS_(id, a)) == 0)
                break;
            if (*(int*)((char*)a + 0x43c) == 6 && *(u16*)((char*)a + 0x496) == 0x64) {
                _ZN7fBase_c18MarkForDestructionEv(((char*)this));
                return 1;
            }
        }
    }

    if (mDelay != 0) {
        u16* p = (u16*)(int)((char*)&mDelay);
        *p = *p - 1;
        return 1;
    }

    _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this) + 0x124, ((char*)this) + 0xdc);
    ((Obj*)((char*)&mModel))->m(0);
    return 1;
}
