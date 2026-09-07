//cpp
// @symbol _ZN17dMg3DEspAnimSet_c6RenderEv
/* recovered: shared common types */
#include "common.h"
#include "dMg3DEspAnimSet_c.h"
extern "C" {

extern Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
}
struct ModelComponents;
struct Obj {
    void* vt;
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4();
    virtual void m(void* arg);
};
#pragma opt_strength_reduction off
void dMg3DEspAnimSet_c::Render()
{
    char *c = (char *)this;
    int i;
    char *r5;
    char *r4;
    void *zero;
    Matrix4x3_FromTranslation(&data_020a0e68, 0x8c000, 0x80000, 0x40000);
    r5 = c;
    r4 = c + 0x12c;
    i = 0;
    zero = (void*)i;
    do {
        if (*(int*)(c + i*4 + 0x168) != 0) {
            *(Matrix4x3*)(r5 + 0x1c) = data_020a0e68;
            ((MaterialChanger*)r4)->Update(*(ModelComponents*)(r5 + 8));
            ((Obj*)r5)->m(zero);
        }
        i++;
        r5 += 0x64;
        r4 += 0x14;
    } while (i < 3);
}
