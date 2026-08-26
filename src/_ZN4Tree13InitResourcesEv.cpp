//cpp
// @symbol _ZN4Tree13InitResourcesEv
/* recovered: real class-form method. Vector3 and Model come from the real
   headers through Tree.h (mModel replaces the old cast-to-shadow at the same
   0xd4 + idx*0x50 address). dCcPos_c is deliberately NOT included and its
   Init call keeps the wrong-looking spelling: the ROM name carries by-value
   class parameters (e.g. Fix12<int>), which mwccarm passes differently at
   the call site, so declaring the true types breaks the byte match. See
   notes/mwccarm-codegen.md 6az. */
#include "Tree.h"

extern "C" void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(void *, const Vector3&, int, int, unsigned int, unsigned int);

extern "C" void* _Znwj(unsigned int);
extern "C" void _ZN8dCcPos_cC1Ev(void*);
extern "C" void Vec3_AsrInPlace(void*, int);
extern int data_ov002_02110a48[];
extern unsigned short data_ov002_0210abb8[];

s32 Tree::InitResources() {
    char* self = (char*)this;
    int idx = ((unsigned int)*(int*)(self + 8) >> 4) & 7;
    int* slot;
    char* p;
    if (idx >= 4) idx = 4;
    slot = &data_ov002_02110a48[idx];
    if (*slot == 0) {
        mModel[idx].LoadAndSetFile(data_ov002_0210abb8[idx], 1, 1);
    }
    p = (char*)_Znwj(0x4c);
    if (p) _ZN8dCcPos_cC1Ev(p + 0xc);
    *(int*)(p + 0) = *(int*)(self + 0x5c);
    *(int*)(p + 4) = *(int*)(self + 0x60);
    *(int*)(p + 8) = *(int*)(self + 0x64);
    Vec3_AsrInPlace(p, 3);
    {
        int* q = (int*)(((int)p + 4));
        *q = *q + 0x1e000;
    }
    _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj((void*)(p + 0xc), *(Vector3*)(self + 0x5c), 0x35555, 0x1f4000, 0x380000c, 0);
    *(int*)(p + 0x48) = *slot;
    *slot = (int)p;
    if (*(int*)(p + 0x48) != 0) {
        return 0;
    } else {
        return 1;
    }
}
