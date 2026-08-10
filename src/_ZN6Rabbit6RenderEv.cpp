//cpp
// @symbol _ZN6Rabbit6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Rabbit.h"
extern "C" {
extern signed char data_0209f2f8;
extern signed char data_02092120;
}

struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m14(void* arg);
};

int Rabbit::Render()
{
    if (unk_428 == 1) return 1;

    {
        int b = (unk_0b0 & 0x40000) != 0;
        if (b) return 1;
    }

    mScale = 0x1500;
    unk_088 = mScale;
    unk_084 = unk_088;

    {
        int** base = (int**)((char*)&unk_308);
        int* r3 = base[0];
        char* r1 = (char*)base[1];
        for (unsigned int i = 0; i < *(unsigned int*)((char*)r3 + 0x24); i++) {
            *(int*)(r1 + 0x20) = unk_468;
            r1 += 0x30;
        }
    }

    if (data_0209f2f8 == 5 && data_02092120 == 3) {
        func_ov085_0212c150(((char*)this));
    }

    ((VObj*)((char*)&mModelAnim))->m14((char*)&mScale);
    return 1;
}
