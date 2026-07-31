//cpp
// @symbol func_ov077_021253a4
/* recovered: shared common types */
#include "common.h"

extern "C" {
extern void _ZN9Animation7AdvanceEv(void*);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
extern void func_020383fc(void*);
extern void func_02038420(void*);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void*);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void*, const Vector3*, unsigned int, int, short);
extern void _ZN5Actor8PoofDustEv(void*);
extern void func_02012694(int a, void* b);
extern void _ZN9ActorBase18MarkForDestructionEv(void*);

int func_ov077_021253a4(char* c)
{
    *(short*)(c + 0x8c) = *(short*)(c + 0x8c) - 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x174);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x1b0);

    if (*(int*)(c + 0x98) >= *(int*)(c + 0x1fc) || *(int*)(c + 0xa8) >= *(int*)(c + 0x1fc)) {
        func_020383fc(c + 0x1e4);
    } else {
        func_02038420(c + 0x1e4);
    }

    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x1e4) || DecIfAbove0_Byte((unsigned char*)(c + 0x3e8)) == 0) {
        Vector3 v;
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, 1, 0x2000, 0);
        _ZN5Actor8PoofDustEv(c);
        func_02012694(0xc4, c + 0x74);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }
    return 1;
}
}
