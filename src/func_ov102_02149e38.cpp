//cpp
// @symbol func_ov102_02149e38
/* recovered: shared common types */
#include "common.h"
extern "C" {

struct MMC { char p[0x124]; };
struct Obj { char p[0x2ec]; Matrix4x3 m; };
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(MMC*, Matrix4x3&, short);
void func_ov102_02149e38(char* self){
    Obj* o = (Obj*)self;
    o->m = *(Matrix4x3*)(self + 0xf0);
    *(int*)(self+0x310) = *(int*)(self+0x5c);
    *(int*)(self+0x314) = *(int*)(self+0x60) + *(int*)(self+0x3dc);
    *(int*)(self+0x318) = *(int*)(self+0x64);
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s((MMC*)(self+0x124), o->m, *(short*)(self+0x8e));
}
}
