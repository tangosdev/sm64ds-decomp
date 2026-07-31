//cpp
// @symbol _ZN15FaderBrightness11AdvanceFadeEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "FaderBrightness.h"
void _ZN5Fader13AdvanceInterpEv(void* thiz);
void func_02053a90(unsigned short* out, int v);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
void _ZN2GX10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);

void FaderBrightness::AdvanceFade()
{
    int old = unk_004;
    int v;
    _ZN5Fader13AdvanceInterpEv(((char*)this));
    if (unk_004 == old) return;
    v = -(unk_004 << 4) >> 0xc;
    func_02053a90((unsigned short*)0x400006c, v);
    func_02053a90((unsigned short*)0x400106c, v);
    if (v > -0x10) return;
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)data_0209d3ac, 2);
    _ZN2GX10LoadBGPlttEPKvjj(data_0209d3ac, 0, 2);
    _ZN3GXS10LoadBGPlttEPKvjj(data_0209d3ac, 0, 2);
}
