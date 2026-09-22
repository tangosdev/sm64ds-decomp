//cpp
/* daTrs_c model/shadow/cap update helper (ov063 0x021166ac, 0x318 bytes), called
 * from daTrs_c::Behavior. State 0xf hands off to func_ov063_0211640c. Otherwise,
 * gated on flag bit 3 of the 0x5d4 halfword: with bit 1 set the body rotation
 * at 0x5ba spins (unless the model file id at 0x4a0 is 0xd4) and the 0x400
 * matrix is rebuilt from it with the 0x504 position; state 3 places the
 * animated model at the actor position and points its bone 0x1a..0x1e at the
 * actor angles (pitch minus 0x4000), any other state rotates the model to the
 * actor yaw; the shadow matrix at 0x4a4 is rebuilt and the shadow dropped with
 * a radius that depends on the actor id (0xd2 is the boss), then the cap is
 * repositioned from the 0x564 position (y offset by 96.0 * the 0x80 scale) and
 * the actor angles.
 *
 * Shapes that were load-bearing: common.h before daTrs_c.h so the flat
 * Matrix4x3 wins (three ldm/stm pairs per 12-word copy; the math/Matrix.h
 * composite copies 9 words plus a Vector3); the four-way `== 3` chain is the
 * ROM's own (see daTrs_c::Render); the DropShadowRadHeight radius is the
 * varying argument and 0xc8000 the height; the equal-arm ternary on the rot
 * argument orders the UpdateCapPos argument setup (r2 before r1). */
/* common.h first so the flat Matrix4x3 (s32 m[12]) wins over math/Matrix.h's
   composite spelling: the 0x39c / 0x4a4 matrix stores are single 12-word copies
   (three ldm/stm pairs), not a 3x3 copy plus a Vector3 copy. */
#include "common.h"
#include "daTrs_c.h"

extern "C" {
void func_ov063_0211640c(char *c);
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angle);
void Vec3_Asr(Vector3 *d, const Vector3 *s, int sh);
void Matrix4x3_FromTranslation(Matrix4x3 *m, s32 x, s32 y, s32 z);
void _ZN9ModelBase12ApplyOpacityEj(void *self, u32 opacity, int enable);
void func_020167a4(void *p);
void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *self);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 angY);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *m, int rad, int h, u32 a);
void _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16(void *self, const Vector3 *pos, const Vector3_16 *rot);
void func_ov063_021160d4(char *c);
extern Matrix4x3 data_020a0e68;
}

extern "C" void func_ov063_021166ac(char *c)
{
    daTrs_c *t = (daTrs_c *)c;
    Vector3_16 rot;
    Vector3 t1;
    Vector3 t2;
    Vector3 pos;

    if (t->unk_5cf == 0xf) {
        func_ov063_0211640c(c);
        return;
    }
    if (!t->mFlags_5d4.b3)
        return;
    if (t->mFlags_5d4.b1) {
        if (*(u16 *)(c + 0x4a0) != 0xd4) {
            s16 *ap = (s16 *)(c + 0x5ba);
            *ap += 0xc00;
        }
        Matrix4x3_FromRotationY((Matrix4x3 *)(c + 0x400), *(s16 *)(c + 0x5ba));
        *(s32 *)(c + 0x424) = *(s32 *)(c + 0x504) >> 3;
        *(s32 *)(c + 0x428) = *(s32 *)(c + 0x508) >> 3;
        *(s32 *)(c + 0x42c) = *(s32 *)(c + 0x50c) >> 3;
    }

    if (t->unk_5cc == 3 || t->unk_5cc == 3 ||
        t->unk_5cc == 3 || t->unk_5cc == 3) {
        Vec3_Asr(&t1, (Vector3 *)(c + 0x5c), 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t1.x, t1.y, t1.z);
        *(Matrix4x3 *)(c + 0x39c) = data_020a0e68;
        _ZN9ModelBase12ApplyOpacityEj(c + 0x380, (u8)(*(u8 *)(c + 0x5c8) >> 3), 1);
        func_020167a4(c + 0x380);
        {
            char *p = *(char **)(c + 0x390);
            *(s16 *)(p + 0x1a) = *(s16 *)(c + 0x8c);
            *(s16 *)(p + 0x1c) = *(s16 *)(c + 0x8e) - 0x4000;
            *(s16 *)(p + 0x1e) = *(s16 *)(c + 0x90);
        }
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(c + 0x388);
    } else {
        Vec3_Asr(&t2, (Vector3 *)(c + 0x5c), 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t2.x, t2.y, t2.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16 *)(c + 0x8e));
        *(Matrix4x3 *)(c + 0x39c) = data_020a0e68;
        _ZN9ModelBase12ApplyOpacityEj(c + 0x380, (u8)(*(u8 *)(c + 0x5c8) >> 3), 1);
    }

    Matrix4x3_FromTranslation(&data_020a0e68, *(s32 *)(c + 0x5c) >> 3, *(s32 *)(c + 0x60) >> 3, *(s32 *)(c + 0x64) >> 3);
    *(Matrix4x3 *)(c + 0x4a4) = data_020a0e68;
    {
        int big = (*(u16 *)(c + 0xc) == 0xd2);
        if (big)
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x434, c + 0x4a4, 0x12c000, 0xc8000, 0xf);
        else
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x434, c + 0x4a4, 0x64000, 0xc8000, 0xf);
    }
    *(s32 *)(c + 0x568) = *(s32 *)(c + 0x538) + (int)(((s64)*(s32 *)(c + 0x80) * 0x60000 + 0x800) >> 12);
    pos.x = *(s32 *)(c + 0x564);
    pos.y = *(s32 *)(c + 0x568);
    pos.z = *(s32 *)(c + 0x56c);
    {
        /* The angle triple copies as two halfword loads, then two stores, then
           the third pair: two named u16 temps for x and y reproduce that; a
           Vector3_16 struct copy or three direct member copies alternate
           load/store (and the s16-typed copy sign-extends, ldrsh). */
        u16 ax = *(u16 *)(c + 0x8c);
        u16 ay = *(u16 *)(c + 0x8e);
        rot.y = ay;
        rot.x = ax;
        rot.z = *(u16 *)(c + 0x90);
    }
    /* equal-arm ternary: the rot address (r2) is set up before pos (r1), as in the ROM */
    _ZN11dCapEnemy_c12UpdateCapPosERK7Vector3RK10Vector3_16(c, &pos, c ? &rot : &rot);
    func_ov063_021160d4(c);
}
