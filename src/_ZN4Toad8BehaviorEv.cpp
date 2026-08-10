//cpp
// @symbol _ZN4Toad8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * Toad watches the player and turns his head. Everything else in the function
 * is bookkeeping around that.
 *
 * The head-turn is two smoothed axes: mHeadYaw chases mHeadYawTarget at 0x250
 * a frame and mHeadPitch chases mHeadPitchTarget at 0x100, and the targets are
 * recomputed each frame from the player's direction. He only tracks while the
 * player is inside the distance threshold AND within 0x3000 of his facing; out
 * of range the targets go to zero and the head returns to centre.
 *
 * The threshold widens from 0xfa000 to 0x1f4000 when data_0209f2f8 is 0x32 --
 * a mode in which he also slowly rotates his whole body toward the player
 * rather than just his head, and in which he never fades out.
 *
 * mAngleY is his facing, and the ApproachLinear on it in that one branch is
 * the only place the body turns.
 *
 * The opacity ramp is the other half: unk_20e is the target and unk_20d the
 * current value, stepped by 6 a frame, and the model's opacity is that value
 * shifted right by 3. Anything that should keep him visible -- being close,
 * having a live tracked actor, or the 0x32 mode -- writes 0xff into the
 * target.
 *
 * The two `s + 0x158` / `s + 0x164` offsets the placeholder body carried were
 * inside mModelAnim: its Animation base sits at +0x50 and that base's speed at
 * +0x5c, so they are member accesses now and the compiler applies the
 * adjustment.
 */
#include "Toad.h"
#include "CylinderClsn.h"

extern "C" {
extern s8 data_0209f2f8;
extern void func_ov085_02129570(void *c);
extern void func_ov085_021295bc(void *c);
extern s32 Vec3_HorzDist(const void *a, const void *b);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern s16 Vec3_VertAngle(const void *a, const void *b);
extern s32 AngleDiff(s32 a, s32 b);
extern void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 rate);
extern void _Z15ApproachLinear2Riii(s32 *dst, s32 target, s32 rate);
extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void *_ZN5Actor13ClosestPlayerEv(void *c);
extern void _ZN9ModelBase12ApplyOpacityEj(void *c, u32 a, int b);
}

int Toad::Behavior()
{
    func_ov085_02129570(this);
    mModelAnim.UpdateVerts();

    char *p = (char *)_ZN5Actor13ClosestPlayerEv(this);
    if (p != 0) {
        s32 threshold = 0xfa000;
        if (data_0209f2f8 == 0x32)
            threshold = 0x1f4000;

        Vector3 *psrc = (Vector3 *)(p + 0x5c);
        Vector3 v;
        v.x = psrc->x;
        v.y = psrc->y;
        v.z = psrc->z;

        s32 hd = Vec3_HorzDist(&mPosX, &v);

        Vector3 v2;
        v2.y = v.y;
        v2.x = v.x;
        v2.z = v.z;
        v2.y = v.y + 0x1e000;

        s16 ha = Vec3_HorzAngle(&mPosX, &v2);
        s16 va = Vec3_VertAngle(&mPosX, &v2);

        if (hd < 0x190000)
            unk_20e = 0xff;
        else
            unk_20e = 0x3c;

        if (hd < threshold && AngleDiff(ha, mAngleY) < 0x3000) {
            mHeadYawTarget = (s16)(ha - mAngleY);
            mHeadPitchTarget = va;
        } else if (data_0209f2f8 != 0x32) {
            mHeadYawTarget = 0;
            mHeadPitchTarget = 0;
        } else {
            _Z14ApproachLinearRsss(&mAngleY, ha, 0x100);
            mHeadYawTarget = 0;
            mHeadPitchTarget = 0;
        }
    } else {
        unk_20e = 0x3c;
    }

    {
        u32 id = unk_1f4;
        if (id != 0) {
            if (_ZN5Actor10FindWithIDEj(id) != 0)
                unk_20e = 0xff;
        }
    }

    if (data_0209f2f8 == 0x32)
        unk_20e = 0xff;

    _Z14ApproachLinearRsss(&mHeadYaw, mHeadYawTarget, 0x250);
    _Z14ApproachLinearRsss(&mHeadPitch, mHeadPitchTarget, 0x100);

    _Z15ApproachLinear2Riii((s32 *)&unk_20d, unk_20e, 6);

    mModelAnim.Advance();

    func_ov085_021295bc(this);

    mModelAnim.speed = 0x1000;

    _ZN9ModelBase12ApplyOpacityEj(&mModelAnim, (u32)(u8)(unk_20d >> 3), 1);

    ((CylinderClsn *)&mMovingCylinderClsn)->Clear();
    ((CylinderClsn *)&mMovingCylinderClsn)->Update();

    return 1;
}
