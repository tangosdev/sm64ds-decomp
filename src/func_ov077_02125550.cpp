//cpp
// @symbol func_ov077_02125550
/* recovered: shared common types, declarations from a shared header */
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"


extern "C" {
extern void func_020383fc(void* p);
extern void func_02038420(void* p);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void* p);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int n, int a, int b, int c);
extern void func_0201267c(int a, void* p);
extern void func_ov077_02125e94(void* c, int a);
extern void _ZN5Actor8PoofDustEv(void* c);
extern void func_02012694(int a, void* p);
extern void _ZN9ActorBase18MarkForDestructionEv(void* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* c, void* p);
extern void _ZN12CylinderClsn5ClearEv(void* p);
extern void _ZN12CylinderClsn6UpdateEv(void* p);
}

extern "C" int func_ov077_02125550(char* c)
{
    Vector3 vec;
    int x, y, z;
    int d;

    *(short*)(c + 0x8c) = *(short*)(c + 0x8c) + 0x4e20;

    if (*(int*)(c + 0x98) >= *(int*)(c + 0x1fc) || *(int*)(c + 0xa8) >= *(int*)(c + 0x1fc)) {
        func_020383fc(c + 0x1e4);
    } else {
        func_02038420(c + 0x1e4);
    }

    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x1e4)) {
        *(int*)(c + 0xa8) = *(int*)(c + 0xa8) * -0x3c / 100;
        if (*(int*)(c + 0xa8) > 0x8000) {
            x = *(int*)(c + 0x5c);
            y = *(int*)(c + 0x60) + 0x28000;
            z = *(int*)(c + 0x64);
            ((int*)&vec)[0] = x;
            ((int*)&vec)[1] = y;
            ((int*)&vec)[2] = z;
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, vec.x, vec.y, vec.z);
            func_0201267c(0x109, c + 0x74);
        } else {
            *(int*)(c + 0xa8) = 0;
            *(short*)(c + 0x8c) = 0;
            _ZN12WithMeshClsn15ClearLimMovFlagEv(c + 0x1e4);
            func_ov077_02125e94(c, 1);
        }
    }

    d = *(int*)(c + 0x3dc) ? *(int*)(c + 0x60) - *(int*)(c + 0x3dc) : 0;
    if (d < -0xc8000) {
        _ZN5Actor8PoofDustEv(c);
        func_02012694(0x166, c + 0x74);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x1b0);
    func_ov077_02124eb0(c);
    _ZN12CylinderClsn5ClearEv(c + 0x1b0);
    _ZN12CylinderClsn6UpdateEv(c + 0x1b0);
    return 1;
}
