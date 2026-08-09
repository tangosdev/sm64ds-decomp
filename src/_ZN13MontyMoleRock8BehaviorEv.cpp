//cpp
// @symbol _ZN13MontyMoleRock8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MontyMoleRock.h"
extern "C" void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *v16, int e, int f);
extern "C" int RandomIntInternal(int *seed);

extern int data_0209e650;

int MontyMoleRock::Behavior()
{
    unsigned char *c = (unsigned char *)((void *)this);
    unsigned char *o;

    Actor_MakeVanishLuigiWork(c, c + 0x160);

    {
        unsigned int id = *(unsigned int *)(c + 0x184);
        if (id != 0) {
            o = (unsigned char *)Actor_FindWithID(id);
            if (o != 0 && (*(int *)(c + 0x180) & 0x400000)) {
                if (*(unsigned char *)(o + 0x6f9) != 0) {
                    int v1[3];
                    v1[0] = *(int *)(c + 0x5c);
                    v1[1] = *(int *)(c + 0x60);
                    v1[2] = *(int *)(c + 0x64);
                    Player_Hurt(o, v1, 1, 0xc000, 1, 0, 1);
                } else if (*(unsigned char *)(o + 0x6fb) == 0) {
                    int v2[3];
                    v2[0] = *(int *)(c + 0x5c);
                    v2[1] = *(int *)(c + 0x60);
                    v2[2] = *(int *)(c + 0x64);
                    Player_Hurt(o, v2, 1, 0xc000, 1, 0, 1);
                }
            }
        }
    }

    if (WithMeshClsn_IsOnGround(c + 0x194) != 0) {
        if (*(unsigned char *)(c + 0x350) == 0) {
            unsigned char *s;
            int r;

            s = (unsigned char *)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x137, 1, c + 0x5c, 0, *(signed char *)(c + 0xcc), -1);
            r = RandomIntInternal(&data_0209e650);
            *(short *)(s + 0x92) = 0;
            *(short *)(s + 0x94) = (short)((unsigned int)r >> 8);
            *(short *)(s + 0x96) = 0;
            *(int *)(s + 0x98) = *(int *)(c + 0x98) >> 1;
            *(int *)(s + 0xa4) = 0;
            *(int *)(s + 0xa8) = 0x5000;
            *(int *)(s + 0xac) = 0;

            s = (unsigned char *)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x137, 1, c + 0x5c, 0, *(signed char *)(c + 0xcc), -1);
            r = RandomIntInternal(&data_0209e650);
            *(short *)(s + 0x92) = 0;
            *(short *)(s + 0x94) = (short)((unsigned int)r >> 8);
            *(short *)(s + 0x96) = 0;
            *(int *)(s + 0x98) = *(int *)(c + 0x98) >> 1;
            *(int *)(s + 0xa4) = 0;
            *(int *)(s + 0xa8) = 0x5000;
            *(int *)(s + 0xac) = 0;
        }
        ActorBase_MarkForDestruction(c);
    }

    Actor_UpdatePos(c, 0);
    Enemy_UpdateWMClsn(c, c + 0x194, 0);
    func_ov080_02124418(c);
    CylinderClsn_Clear(c + 0x160);
    CylinderClsn_Update(c + 0x160);
    return 1;
}
