#include "types.h"
// @symbol func_ov002_020b38a0
// recovered name: BigBrickBlock_Kill
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* daObjBlockL_c::Kill - recovered from vtable slot identity */
extern void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h(char *thiz, s8 *ref, u32 b, const struct Vector3 *v, u32 j);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const struct Vector3 *v, const void *v16, s32 e, s32 f);
extern void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(char *thiz, const struct Vector3 *v, u32 n, s32 f, short s);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 kind, s32 x, s32 y, s32 z);
extern void _ZN5Actor10PoofDustAtERK7Vector3(char *thiz, const struct Vector3 *v);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, const struct Vector3 *v);
extern void _ZN5Actor13SpawnSoundObjEj(char *thiz, u32 a);
extern void _ZN9ActorBase18MarkForDestructionEv(char *thiz);

void func_ov002_020b38a0(char *c)
{
    struct Vector3 tmp[4];
    u32 kind;
    u32 is11;
    u32 isF;
    u32 f11;
    u32 f2e;
    u32 f13;

    is11 = (*(u16 *)(c + 0xc) == 0x11);
    if (is11 && *(u8 *)(c + 0x322) != 0) {
        tmp[0].x = *(s32 *)(c + 0x5c);
        tmp[0].y = *(s32 *)(c + 0x60);
        tmp[0].z = *(s32 *)(c + 0x64);
        tmp[0].y += 0x96000;

        if (((*(u32 *)(c + 8) >> 8) & 0xff) == 0xff) {
            _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h(c, (s8 *)(c + 0x323), *(u8 *)(c + 0x322), &tmp[0], 4);
        } else {
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb2, *(u8 *)(c + 0x322) | 0x40, &tmp[0], 0, *(s8 *)(c + 0xcc), -1);
        }
    } else {
        isF = (*(u16 *)(c + 0xc) == 0xf);
        if (isF || is11) {
            if (*(u32 *)(c + 0x328) == 0) {
                tmp[2].x = *(s32 *)(c + 0x5c);
                tmp[2].y = *(s32 *)(c + 0x60);
                tmp[2].z = *(s32 *)(c + 0x64);
                _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &tmp[2], 3, 0xf000, 0);
            }
        }
    }

    {
        s32 vz = *(s32 *)(c + 0x64);
        s32 vy = *(s32 *)(c + 0x60) + 0x64000;
        s32 vx = *(s32 *)(c + 0x5c);
        tmp[1].x = vx;
        tmp[1].z = vz;
        tmp[1].y = vy;
    }

    kind = 0xa;
    f11 = (*(u16 *)(c + 0xc) == 0x11);
    if (f11) {
        tmp[1].y = *(s32 *)(c + 0x60) + 0x96000;
        kind = 0xf;
    }
    f2e = (*(u16 *)(c + 0xc) == 0x2e);
    if (f2e) {
        kind = 0x48;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kind, tmp[1].x, tmp[1].y, tmp[1].z);

    tmp[3].x = tmp[1].x;
    tmp[3].y = tmp[1].y;
    tmp[3].z = tmp[1].z;
    _ZN5Actor10PoofDustAtERK7Vector3(c, &tmp[3]);

    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, (const struct Vector3 *)(c + 0x74));

    func_ov002_020b363c(c);

    f13 = (*(u16 *)(c + 0xc) == 0x13);
    if (f13) {
        *(u8 *)(c + 0x31e) = 1;
        return;
    }

    if (func_ov002_020b36a0(c) != 0) {
        _ZN5Actor13SpawnSoundObjEj(c, 1);
    }
    _ZN9ActorBase18MarkForDestructionEv(c);
}
