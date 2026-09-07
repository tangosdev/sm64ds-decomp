//cpp
// @symbol _ZN12WaterSuction13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "WaterSuction.h"
extern "C" {
struct V3 { int x, y, z; };
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* thiz, void* actor, V3* vec, int a, int b, unsigned int cc, unsigned int d);
extern V3 data_ov026_02113f4c;
}

int WaterSuction::InitResources()
{
    V3 vec;
    unk_314 = (*(s32 *)&param1) & 0xff;
    vec = data_ov026_02113f4c;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(((char*)this) + 0x110, ((char*)this), &vec, 0x64000, 0xa4000, 0x800006, 0);
    func_ov026_021122d4(((char*)this), &data_ov026_02113f58);
    return 1;
}
