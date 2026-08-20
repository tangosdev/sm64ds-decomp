//cpp
// @symbol _ZN13MontyMoleRock8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MontyMoleRock.h"
extern "C" void _ZN7fBase_c18MarkForDestructionEv(void*);
extern "C" void* _ZN8dActor_c10FindWithIDEj(unsigned int);
extern "C" void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(void*, void*);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(void*, void*);
extern "C" void _ZN5dCc_c5ClearEv(void*);
extern "C" void _ZN5dCc_c6UpdateEv(void*);
extern "C" void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void*, void*, unsigned int);
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void*, void*, unsigned int, int, unsigned int, unsigned int, unsigned int);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern "C" void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *v16, int e, int f);
extern "C" int RandomIntInternal(int *seed);

extern int data_0209e650;

s32 MontyMoleRock::Behavior()
{
    unsigned char *c = (unsigned char *)((void *)this);
    unsigned char *o;

    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(c, c + 0x160);

    {
        unsigned int id = *(unsigned int *)(c + 0x184);
        if (id != 0) {
            o = (unsigned char *)_ZN8dActor_c10FindWithIDEj(id);
            if (o != 0 && (*(int *)(c + 0x180) & 0x400000)) {
                if (*(unsigned char *)(o + 0x6f9) != 0) {
                    int v1[3];
                    v1[0] = *(int *)(c + 0x5c);
                    v1[1] = *(int *)(c + 0x60);
                    v1[2] = *(int *)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(o, v1, 1, 0xc000, 1, 0, 1);
                } else if (*(unsigned char *)(o + 0x6fb) == 0) {
                    int v2[3];
                    v2[0] = *(int *)(c + 0x5c);
                    v2[1] = *(int *)(c + 0x60);
                    v2[2] = *(int *)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(o, v2, 1, 0xc000, 1, 0, 1);
                }
            }
        }
    }

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194) != 0) {
        if (*(unsigned char *)(c + 0x350) == 0) {
            unsigned char *s;
            int r;

            s = (unsigned char *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x137, 1, c + 0x5c, 0, *(signed char *)(c + 0xcc), -1);
            r = RandomIntInternal(&data_0209e650);
            *(short *)(s + 0x92) = 0;
            *(short *)(s + 0x94) = (short)((unsigned int)r >> 8);
            *(short *)(s + 0x96) = 0;
            *(int *)(s + 0x98) = *(int *)(c + 0x98) >> 1;
            *(int *)(s + 0xa4) = 0;
            *(int *)(s + 0xa8) = 0x5000;
            *(int *)(s + 0xac) = 0;

            s = (unsigned char *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x137, 1, c + 0x5c, 0, *(signed char *)(c + 0xcc), -1);
            r = RandomIntInternal(&data_0209e650);
            *(short *)(s + 0x92) = 0;
            *(short *)(s + 0x94) = (short)((unsigned int)r >> 8);
            *(short *)(s + 0x96) = 0;
            *(int *)(s + 0x98) = *(int *)(c + 0x98) >> 1;
            *(int *)(s + 0xa4) = 0;
            *(int *)(s + 0xa8) = 0x5000;
            *(int *)(s + 0xac) = 0;
        }
        _ZN7fBase_c18MarkForDestructionEv(c);
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, c + 0x194, 0);
    func_ov080_02124418(c);
    _ZN5dCc_c5ClearEv(c + 0x160);
    _ZN5dCc_c6UpdateEv(c + 0x160);
    return 1;
}
