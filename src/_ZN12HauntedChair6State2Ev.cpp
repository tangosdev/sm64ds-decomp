//cpp
// @symbol _ZN12HauntedChair6State2Ev
#include "HauntedChair.h"

typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef struct { s32 x, y, z; } PlainVector3;

extern "C" {
extern void dBgCh_Actr_UpdateContinuous_Veneer(void *);
extern void func_0201267c(int, void *);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *);
extern void Vec3_Sub(PlainVector3 *res, const PlainVector3 *v0, const PlainVector3 *v1);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern s32 Vec3_HorzLen(const PlainVector3 *v0);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *);
extern void func_ov020_021131f8(void *);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void *);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *, PlainVector3 *);
extern int _ZN8dActor_c14GetSubtractionEss(void *, s16, s16);
extern void AddVec3(void *, void *, void *);
extern void *_ZN8dActor_c10FindWithIDEj(u32);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *, const PlainVector3 *, u32, int, u32, u32, u32);

extern s16 data_02082214[];
}

typedef struct {
    u8 _pad[0x9e];
    u16 counter;
    u16 timer;
} State300;

#define A92 (*(u16 *)((char *)c + 0x92))
#define A94 (*(u16 *)((char *)c + 0x94))
#define ST  ((State300 *)((char *)c + 0x300))

void HauntedChair::State2()
{
    char *c = (char *)this;
    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x1bc);
    (*(u16 *)(((int)c + 0x39e)))++;

    if (ST->counter <= 0x46) {
        if (ST->counter < 0x32)
            *(s32 *)(c + 0xa8) = 0x6000;
        else
            *(s32 *)(c + 0xa8) = 0;
        ApproachStateValue((short *)(c + 0x8c), (short *)(c + 0x398),
                           -0xfa0, 0xc8, 0x14, 2);
        ApproachStateValue((short *)(c + 0x90), (short *)(c + 0x39c),
                           0, 0, 0x14, 1);
        if (ST->counter == 0x46) {
            func_0201267c(0x5c, c + 0x74);
        }
    } else {
        if (ST->timer != 0) {
            (*(u16 *)(((int)c + 0x3a0)))--;
            if (ST->timer == 0) {
                void *player;
                func_0201267c(0x5d, c + 0x74);
                player = _ZN8dActor_c13ClosestPlayerEv(c);
                if (player == 0)
                    return;
                {
                    PlainVector3 *pp = (PlainVector3 *)(((int)player + 0x5c));
                    PlainVector3 playerPos;
                    PlainVector3 diff;
                    s32 r;
                    playerPos.x = pp->x;
                    playerPos.y = pp->y;
                    playerPos.z = pp->z;
                    Vec3_Sub(&diff, &playerPos, (PlainVector3 *)(c + 0x5c));
                    *(s16 *)(c + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(diff.x, diff.z);
                    *(s16 *)(c + 0x92) = _ZN4cstd5atan2E5Fix12IiES1_(diff.y, Vec3_HorzLen(&diff)) * -1;
                    r = (s32)(((long long)data_02082214[(A92 >> 4) * 2 + 1] * 0x32000 + 0x800) >> 12);
                    *(s32 *)(c + 0xa4) = (s32)(((long long)r * data_02082214[(A94 >> 4) * 2] + 0x800) >> 12);
                    *(s32 *)(c + 0xa8) = (s32)(((long long)data_02082214[(A92 >> 4) * 2] * -0x32000 + 0x800) >> 12);
                    *(s32 *)(c + 0xac) = (s32)(((long long)r * data_02082214[(A94 >> 4) * 2 + 1] + 0x800) >> 12);
                }
            } else {
                if (ST->timer > 0x14) {
                    *(s16 *)(((int)c + 0x8e)) += 0x2710;
                }
            }
        } else {
            if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1bc) != 0) {
                Break();
            } else if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x1bc) != 0) {
                void *wr = _ZNK10dBgCh_Actr13GetWallResultEv(c + 0x1bc);
                PlainVector3 normal;
                _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)wr + 4, &normal);
                if (_ZN8dActor_c14GetSubtractionEss(c, *(s16 *)(c + 0x94),
                        _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z)) > 0x4000) {
                    Break();
                }
            }
        }
    }

    AddVec3(c + 0x5c, c + 0xa4, c + 0x5c);

    {
        u32 id = *(u32 *)(c + 0x1a0);
        void *a;
        int b;
        if (id == 0)
            return;
        a = _ZN8dActor_c10FindWithIDEj(id);
        if (a == 0)
            return;
        b = (int)(*(u16 *)((char *)a + 0xc) == 0xbf);
        if (b == 0)
            return;
        b = *(u8 *)((char *)a + 0x6fb);
        if (b != 0)
            return;
        {
            PlainVector3 pos;
            pos.x = *(s32 *)(c + 0x5c);
            pos.y = *(s32 *)(c + 0x60);
            pos.z = *(s32 *)(c + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &pos, 2, 0xc000, 1, 0, 1);
        }
    }
}
