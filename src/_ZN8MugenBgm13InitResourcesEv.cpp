//cpp
// @symbol _ZN8MugenBgm13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MugenBgm.h"
extern "C" {
void* _Znwj(unsigned int);
void _ZN3Fog4InitEt5Fix12IiES1_(void* thiz, unsigned short a, int b, int d);
}

int MugenBgm::InitResources()
{
    void* p;
    /* The param1 mask reads through a const view of `this`: `param1 &= 0xf`
       makes mwcc CSE the +8 field address into r1 (add r1,r4,#8 / ldr r0,[r1] /
       str r0,[r1]) and the function grows a word; the cartridge re-issues
       ldr/str [r4,#8]. Same lever as dScMgPachinko2_c::OnYoshiTryEat. */
    const MugenBgm* ro = this;
    param1 = ro->param1 & 0xf;
    if (param1 != 1) {
        data_ov002_02110af4 = mAngleY;
        p = _Znwj(0x28);
        data_ov002_02110af8 = p;
        if (p != 0) {
            _ZN3Fog4InitEt5Fix12IiES1_(p, 0, 0x700, 0xd00);
        }
        data_ov002_02110af0 = mAreaId;
    }
    mAreaId = -1;
    mAngleY = -mAngleY;
    return 1;
}
