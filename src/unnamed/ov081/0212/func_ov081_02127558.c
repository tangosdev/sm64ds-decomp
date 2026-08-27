#include "types.h"
extern int _Z15ApproachLinear2Riii(int* p, int a, int b);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, void* actor, int a, int b, unsigned int c, unsigned int d);
extern int _ZN8dActor_c13DistToCPlayerEv(void* thiz);
extern void func_0201267c(int a, void* p);
extern void func_ov081_0212777c(void* c, int n);
extern void func_ov081_02126758(void* c);
extern void _ZN5dCc_c5ClearEv(void* thiz);
extern void _ZN5dCc_c6UpdateEv(void* thiz);

int func_ov081_02127558(char* c)
{
    u8* pstate;
    int* pflg;

    switch (*(u8*)(c + 0x3f1)) {
    case 0:
        if (_Z15ApproachLinear2Riii((int*)(c + 0x3f0), 1, 2) != 0) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x1b0, c, 0x4b000, 0x73000, 0x200000, 0);
            pflg = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
            *pflg = *pflg & ~0x10000000;
            pstate = (u8*)(((int)c + 0x3f1) & 0xFFFFFFFFFFFFFFFFLL);
            *pstate = *pstate + 1;
        }
        break;
    case 1:
        if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x1f4000) {
            pflg = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
            *pflg = *pflg | 0x10000000;
            func_0201267c(0x76, c + 0x74);
            pstate = (u8*)(((int)c + 0x3f1) & 0xFFFFFFFFFFFFFFFFLL);
            *pstate = *pstate + 1;
        }
        break;
    case 2:
        if (_Z15ApproachLinear2Riii((int*)(c + 0x3f0), 0x1f, 2) != 0) {
            _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x1b0, c, 0x4b000, 0x73000, 0x200000, 0x6eff0);
            func_ov081_0212777c(c, 3);
        }
        break;
    }

    /* Written as an explicit read-modify-write, not `+= 0xc00`, and against the flat
       +0x3ec rather than a pre-split base. `ldrsh`/`strh` carry only an 8-bit offset,
       so 0x3ec cannot be encoded and the address MUST be split; the ROM splits it
       `add rN,c,#0x300` + `[rN,#0xec]`. Under 2004/b56 the compound-assignment form is
       the one and only spelling that instead materialises the whole 0x3ec and loads
       through `[rN]` -- verified in isolation against both compilers. Letting b56 do
       its own splitting reproduces the ROM; pre-splitting it in the source does not. */
    {
        s16 v = *(s16*)(c + 0x3ec);
        *(s16*)(c + 0x3ec) = (s16)(v + 0xc00);
    }
    func_ov081_02126758(c);
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}
