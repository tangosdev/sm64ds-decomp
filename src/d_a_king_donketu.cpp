//cpp
#include "ChiefChilly.h"
extern "C" void* _ZN7fBase_cnwEj(unsigned int sz);
extern "C" void _ZN12dEnemyBase_cC2Ev(void*);
extern "C" void _ZN10dCcAcPos_cC1Ev(void*);
extern "C" void _ZN10dBgCh_ActrC1Ev(void*);
extern "C" void _ZN14BlendModelAnimC1Ev(void*);
extern "C" void _ZN11ShadowModelC1Ev(void*);
extern "C" void func_020733a8(void* p, int a, int b, void* f1, void* f2);

extern int _ZTV11ChiefChilly;
extern "C" void _ZN7Vector3D1Ev(void);
extern "C" void func_0203d384(void);

/* Reconstructed source-style name: SM64DS proves daKing_Donketu_c through RTTI,
 * allocation size, vtable identity, and the KING_DONKETU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ChiefChilly_Spawn. */
extern "C" void* daKing_Donketu_c_classInit(void)
{
    char* p = (char*)_ZN7fBase_cnwEj(sizeof(struct ChiefChilly));
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        *(void**)p = &_ZTV11ChiefChilly;
        _ZN10dCcAcPos_cC1Ev(p + 0x110);
        _ZN10dBgCh_ActrC1Ev(p + 0x150);
        _ZN14BlendModelAnimC1Ev(p + 0x30c);
        _ZN11ShadowModelC1Ev(p + 0x380);
        func_020733a8(p + 0x3e8, 8, 0xc, (void*)&func_0203d384, (void*)&_ZN7Vector3D1Ev);
        func_020733a8(p + 0x448, 8, 0xc, (void*)&func_0203d384, (void*)&_ZN7Vector3D1Ev);
        func_020733a8(p + 0x4d4, 2, 0xc, (void*)&func_0203d384, (void*)&_ZN7Vector3D1Ev);
    }
    return p;
}
