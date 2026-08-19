//cpp
#include "types.h"

extern "C" {
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern s32 Vec3_HorzDist(const void *a, const void *b);
extern s32 Vec3_Dist(const void *a, const void *b);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, const void *v, s32 e);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern void _Z14ApproachLinearRsss(s16 *v, s16 target, s16 step);
extern s32 func_ov002_020de328(void *player);
extern void _ZN8dActor_c9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void WithMeshClsn_UpdateContinuous_Veneer(void *p);
extern s32 _ZNK12WithMeshClsn8IsOnWallEv(void *p);
extern void *_ZNK12WithMeshClsn13GetWallResultEv(void *p);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *s, s32 *out);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern void func_ov096_02136e54(void *self, s32 n);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, s32 x, s32 y, s32 z, const void *v, void *cb);

void func_ov096_02137088(void *self);
}

#define M(p) ((long long)(int)(p))

void func_ov096_02137088(void *self)
{
    char *c = (char *)self;
    s32 pos[3];
    s32 n[3];
    char *player;
    void *p33c;
    s16 ang;
    u16 *p354 = (u16 *)(int)M(c + 0x354);

    *p354 = (u16)(*p354 + 1);

    ang = Vec3_HorzAngle(c + 0x5c, c + 0x340);
    *(s16 *)(c + 0x356) = ang;

    *(u32 *)(c + 0x36c) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(u32 *)(c + 0x36c), 3, 0x85, c + 0x74, 0);

    *(s32 *)(c + 0x98) = 0x14000;

    player = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (player == 0)
        goto null_player;

    {
        s32 *pp = (s32 *)(int)M(player + 0x5c);
        pos[0] = pp[0];
        pos[1] = pp[1];
        pos[2] = pp[2];
    }

    if (Vec3_HorzDist(c + 0x340, pos) < *(s32 *)(c + 0x34c)
        && *(u8 *)(c + 0x360) == 0
        && *(u16 *)(c + 0x354) < 0x384) {
        ang = Vec3_HorzAngle(c + 0x5c, pos);
        *(s16 *)(c + 0x358) = ang;
        _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x358), 0x200);
        p33c = *(void **)(c + 0x33c);
        if (p33c != 0) {
            if (func_ov002_020de328(p33c) != 0) {
                u8 *pb = (u8 *)(int)M(c + 0x360);
                *pb = (u8)(*pb + 1);
            }
        }
    } else {
        _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x356), 0x200);
        if (Vec3_HorzDist(c + 0x340, c + 0x5c) < 0xc8000)
            *(s32 *)(c + 0x35c) = 2;
    }
    goto cont;

null_player:
    *(s32 *)(c + 0x35c) = 2;
    return;

cont:
    if (Vec3_Dist(c + 0x5c, pos) > 0xbb8000
        || *(u16 *)(c + 0x354) >= 0x384) {
        *(s32 *)(c + 0x35c) = 2;
    }

    _ZN8dActor_c9UpdatePosEP12CylinderClsn(c, 0);
    WithMeshClsn_UpdateContinuous_Veneer(c + 0x108);
    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x108) != 0) {
        void *wr = _ZNK12WithMeshClsn13GetWallResultEv(c + 0x108);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)wr + 4, n);
        *(s16 *)(c + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(n[0], n[2]);
    }

    func_ov096_02136e54(c, 0x1000);

    {
        s32 z = *(s32 *)(c + 0x64);
        *(u32 *)(c + 0x364) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32 *)(c + 0x364), 0x11f,
            *(s32 *)(c + 0x5c), *(s32 *)(c + 0x60), z, 0, 0);
        z = *(s32 *)(c + 0x64);
        *(u32 *)(c + 0x368) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32 *)(c + 0x368), 0x120,
            *(s32 *)(c + 0x5c), *(s32 *)(c + 0x60), z, 0, 0);
    }
}
