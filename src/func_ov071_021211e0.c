/* MrI (ov071) tracking behaviour: turns toward the player, fires a bullet actor
 * (0x108) from its facing when the state byte reaches 7, and hands off to the
 * timeout/distance/blocked-by-terrain exits. The three target vectors are built
 * with the same load-then-assign shape: the second one must be spelled exactly
 * like the first or the pointer/y registers swap (the previous draft used
 * volatile field stores there and sat at div 7). */
#include "common.h"

extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern s16 Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
extern void _Z11UpdateAngleRssis(s16 *p, s16 tgt, int div, s16 maxStep);
extern u8 DecIfAbove0_Byte(u8 *p);
extern int func_ov071_02120a20(void *c);
extern void func_ov071_021209c8(void *c);
extern int func_ov071_02120860(void *c);
extern int func_ov071_0212070c(void *c);
extern void func_ov071_02121634(void *c, int a);
extern void func_ov071_02120b14(void *c);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32, u32, const Vector3 *, const Vector3_16 *, s8, s16);
extern void func_0201267c(u32 id, void *pos);
extern s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern int _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(void *self, Vector3 *a, Vector3 *b, int flag);
extern s16 data_02082214[];

int func_ov071_021211e0(char *c)
{
    Vector3_16 rot;
    Vector3 target1;
    Vector3 target2;
    Vector3 pos;
    Vector3 target3;
    s16 ang;

    {
        s32 *pl;
        s32 py;
        s32 pz;
        s32 yoff;

        pl = *(s32 **)(c + 0x1ec);
        py = pl[0x60 / 4];
        pz = pl[0x64 / 4];
        yoff = py + 0x78000;
        target1.x = pl[0x5c / 4];
        target1.y = yoff;
        target1.z = pz;
    }

    Vec3_HorzAngle((Vector3 *)(c + 0x5c), &target1);
    ang = Vec3_VertAngle((Vector3 *)(c + 0x5c), &target1);
    _Z11UpdateAngleRssis((s16 *)(c + 0x8c), ang, 2, 0x320);
    ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &target1);
    _Z11UpdateAngleRssis((s16 *)(c + 0x8e), ang, 2, 0x8fc);

    if ((s16)(*(s16 *)(c + 0x8e) - *(s16 *)(c + 0x20c)) == 0) {
        if (DecIfAbove0_Byte((u8 *)(c + 0x213)) == 0 && func_ov071_02120a20(c) != 0)
            *(u8 *)(c + 0x213) = 0x53;
        *(u8 *)(c + 0x212) = 0xf0;
    }

    if (*(u8 *)(c + 0x214) == 7) {
        s32 *pl2;
        s32 py2;
        s32 pz2;
        s32 yoff2;

        pl2 = *(s32 **)(c + 0x1ec);
        py2 = pl2[0x60 / 4];
        pz2 = pl2[0x64 / 4];
        yoff2 = py2 + 0x4b000;
        target2.x = pl2[0x5c / 4];
        target2.y = yoff2;
        target2.z = pz2;

        {
            s32 px;
            s32 pz2;
            s32 scale;
            s32 round;
            int idx;
            s16 s;
            int isBig;
            int param;

            px = *(s32 *)(c + 0x5c);
            scale = 0x50000;
            pos.x = px;
            pos.y = *(s32 *)(c + 0x60);
            pz2 = *(s32 *)(c + 0x64);
            round = 0x800;
            pos.z = pz2;

            rot = *(Vector3_16 *)(c + 0x8c);

            idx = *(u16 *)(c + 0x8e) >> 4;
            s = data_02082214[idx * 2];
            pos.x = px + (s32)(((s64)s * scale + round) >> 12);

            idx = *(u16 *)(c + 0x8e) >> 4;
            s = data_02082214[idx * 2 + 1];
            pos.z = pz2 + (s32)(((s64)s * scale + round) >> 12);

            rot.x = Vec3_VertAngle(&pos, &target2);

            isBig = (int)(*(u16 *)(c + 0xc) == 0x107);
            if (isBig != 0)
                param = 1;
            else
                param = 0;
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x108, param, &pos, &rot, *(s8 *)(c + 0xcc), -1);
            func_0201267c(0x165, c + 0x74);
            *(u8 *)(c + 0x216) = 0x2e;
            *(u8 *)(c + 0x212) = 0xf0;
            *(u8 *)(c + 0x213) = 0x53;
            *(s32 *)(c + 0x1f4) = 0;
            func_ov071_021209c8(c);
        }
    }

    func_ov071_02120860(c);

    {
        s32 *p3;
        s32 y3;
        s32 z3;
        s32 yoff3;

        p3 = *(s32 **)(c + 0x1ec);
        y3 = p3[0x60 / 4];
        z3 = p3[0x64 / 4];
        yoff3 = y3 + 0x8c000;
        target3.x = p3[0x5c / 4];
        target3.y = yoff3;
        target3.z = z3;
    }

    if (func_ov071_0212070c(c) != 0) {
        func_ov071_02121634(c, 2);
    } else if (Vec3_Dist((Vector3 *)(c + 0x5c), &target1) > 0x5dc000) {
        func_ov071_02121634(c, 0);
    } else if (*(u8 *)(*(char **)(c + 0x1ec) + 0x6fb) != 0) {
        func_ov071_02121634(c, 0);
    } else if (_ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(c, &target3, (Vector3 *)(c + 0x5c), 0) != 0) {
        func_ov071_02121634(c, 0);
    }

    func_ov071_02120b14(c);
    return 1;
}
