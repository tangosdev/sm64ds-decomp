#include "types.h"
// @symbol func_ov063_0211640c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#pragma opt_propagation off
extern void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void _ZN9ModelBase12ApplyOpacityEj(void *self, u32 a, int z);
extern void func_020167a4(void *p);
extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *p);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 ang);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, int a, int b, u32 g);

extern struct Matrix4x3 data_020a0e68;

void func_ov063_0211640c(char *c)
{
    struct Vector3 pos, t1, t2;
    s16 ang;

    if (((u32)(*(u16 *)(c + 0x5d4) << 0x1c) >> 0x1f) == 0)
        return;

    pos.x = *(s32 *)(c + 0x5c);
    pos.y = *(s32 *)(c + 0x60);
    pos.z = *(s32 *)(c + 0x64);
    ang = *(s16 *)(c + 0x8e);

    if ((u32)(*(u16 *)(c + 0x5d4) << 0x17) >> 0x1f) {
        s16 neg = -1;
        s16 a = ang;
        pos.x = pos.x * (int)neg;
        ang = (s16)(a * neg);
    }

    if (*(u8 *)(c + 0x5cc) == 3) {
        Vec3_Asr(&t1, &pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t1.x, t1.y, t1.z);
        *(struct Matrix4x3 *)(c + 0x39c) = data_020a0e68;
        _ZN9ModelBase12ApplyOpacityEj(c + 0x380, (u8)((int)*(u8 *)(c + 0x5c8) >> 3), 1);
        func_020167a4(c + 0x380);
        {
            char *m = *(char **)(c + 0x390);
            *(s16 *)(m + 0x1a) = *(s16 *)(c + 0x8c);
            *(s16 *)(m + 0x1c) = (s16)(ang - 0x4000);
            *(s16 *)(m + 0x1e) = *(s16 *)(c + 0x90);
        }
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(c + 0x388);
    } else {
        Vec3_Asr(&t2, &pos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t2.x, t2.y, t2.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, ang);
        *(struct Matrix4x3 *)(c + 0x39c) = data_020a0e68;
        _ZN9ModelBase12ApplyOpacityEj(c + 0x380, (u8)((int)*(u8 *)(c + 0x5c8) >> 3), 1);
    }

    if (*(u8 *)(c + 0x5c8) >= 0x10) {
        Matrix4x3_FromTranslation(&data_020a0e68,
            pos.x >> 3, pos.y >> 3, pos.z >> 3);
        *(struct Matrix4x3 *)(c + 0x4a4) = data_020a0e68;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x434, c + 0x4a4, 0x12c000, 0xc8000, 0xf);

        if ((u32)(*(u16 *)(c + 0x5d4) << 0x17) >> 0x1f) {
            int px = pos.x;
            int neg = -1;
            pos.x = px * neg;
            Matrix4x3_FromTranslation(&data_020a0e68,
                pos.x >> 3, pos.y >> 3, pos.z >> 3);
            *(struct Matrix4x3 *)(c + 0x4d4) = data_020a0e68;
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x45c, c + 0x4d4, 0x12c000, 0xc8000, 0xf);
        }
    }

    func_ov063_021160d4(c);
}