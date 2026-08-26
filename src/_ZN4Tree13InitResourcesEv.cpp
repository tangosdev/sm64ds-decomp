//cpp
// @symbol _ZN4Tree13InitResourcesEv
#include "Tree.h"

/* Keep this low-level declaration even though dCcPos_c has a class model: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which mwccarm
   homes differently when defined/called through the honest method signature.
   See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(void *, const Vector3&, int, int, unsigned int, unsigned int);

extern "C" void* _Znwj(unsigned int);
extern "C" void _ZN8dCcPos_cC1Ev(void*);
extern "C" void Vec3_AsrInPlace(void*, int);
extern int data_ov002_02110a48[];
extern unsigned short data_ov002_0210abb8[];

int Tree::InitResources()
{
    int idx = (param1 >> 4) & 7;
    int* slot;
    char* p;
    if (idx >= 4) idx = 4;
    slot = &data_ov002_02110a48[idx];
    if (*slot == 0) {
        mModel[idx].LoadAndSetFile(data_ov002_0210abb8[idx], 1, 1);
    }
    p = (char*)_Znwj(0x4c);
    if (p) _ZN8dCcPos_cC1Ev(p + 0xc);
    *(int*)(p + 0) = mPosX;
    *(int*)(p + 4) = mPosY;
    *(int*)(p + 8) = mPosZ;
    Vec3_AsrInPlace(p, 3);
    {
        int* q = (int*)(((int)p + 4));
        *q = *q + 0x1e000;
    }
    _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(p + 0xc, *(Vector3*)&mPosX, 0x35555, 0x1f4000, 0x380000c, 0);
    *(int*)(p + 0x48) = *slot;
    *slot = (int)p;
    if (*(int*)(p + 0x48) != 0) {
        return 0;
    } else {
        return 1;
    }
}
