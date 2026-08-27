//cpp
#include "types.h"
// @symbol _ZN9Butterfly6State7Ev
/* recovered: shared common types */
#include "common.h"
#include "Butterfly.h"
extern "C" {
void _Z14ApproachLinearRiii(int* p, int a, int b);
void* _ZN8dActor_c13ClosestPlayerEv(void* self);
s16 _ZN8dActor_c18HorzAngleToCPlayerEv(void* self);
void _Z14ApproachLinearRsss(s16* p, s16 a, s16 b);
s16 Vec3_VertAngle(void* v1, void* v0);
void dBgCh_Actr_UpdateContinuous_Veneer(void* c);
int _ZNK10dBgCh_Actr8IsOnWallEv(void* c);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* c);
int func_02035638(void* c);
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* self, struct Vector3* pos, unsigned int a, int b, unsigned int c, unsigned int d, unsigned int e);
void _ZN8dActor_c14TriplePoofDustEv(void* self);
void _ZN7fBase_c18MarkForDestructionEv(void* self);
void _ZN5dCc_c5ClearEv(void* c);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* c, struct Vector3* v);
void _ZN5dCc_c6UpdateEv(void* c);
}

extern s16 data_02082214[];

#define L(p) ((int)(p))

void Butterfly::State7()
{
    char* c = (char*)this;
    void* player;
    int r5;
    int v;

    if (*(int*)(c + 0x3e8) > 0x78) {
        int ang = (short)*(s16*)(c + 0x3ee);
        ang = (unsigned short)(short)ang;
        int idx = ang >> 4;
        v = (int)(((s64)(int)data_02082214[(idx << 1) + 1] * 0xa3 + 0x800) >> 12);
        if (v > 0) {
            v = (int)(((s64)v * 0x4800 + 0x800) >> 12);
            *(u16*)L(c + 0x3ee) += 0x2710;
        } else {
            *(u16*)L(c + 0x3ee) += 0xfa0;
        }
        *(int*)L(c + 0x3e0) += v;
    }

    _Z14ApproachLinearRiii((int*)(c + 0x98), 0x14000, 0x1000);

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    if (player != 0) {
        _Z14ApproachLinearRsss((s16*)(c + 0x94), _ZN8dActor_c18HorzAngleToCPlayerEv(c), 0x320);
        _Z14ApproachLinearRsss((s16*)(c + 0x92), Vec3_VertAngle(c + 0x5c, (char*)player + 0x5c), 0x320);
    }

    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x1d8);

    {
        int noId = (*(int*)(c + 0x3b8) == 0);
        r5 = (noId == 0);
    }
    if (r5 == 0) {
        if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x1d8) == 0) {
            if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1d8) == 0) {
                if (func_02035638(c + 0x1d8) == 0) {
                    if (*(int*)(c + 0x3e8) <= 0x9d)
                        goto cylinder_only;
                }
            }
        }
    }

    if (r5 != 0) {
        void* a = _ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c + 0x3b8));
        if (a != 0) {
            int isPlayer = (*(u16*)((char*)a + 0xc) == 0xbf);
            if (isPlayer != 0) {
                struct Vector3 pos;
                pos.x = *(int*)(c + 0x5c);
                pos.y = *(int*)(c + 0x60);
                pos.z = *(int*)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &pos, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    _ZN8dActor_c14TriplePoofDustEv(c);
    _ZN7fBase_c18MarkForDestructionEv(c);

cylinder_only:
    _ZN5dCc_c5ClearEv(c + 0x394);
    {
        struct Vector3 off;
        off.x = 0;
        off.y = -0x32000;
        off.z = 0;
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x394, &off);
    }
    _ZN5dCc_c6UpdateEv(c + 0x394);
}
