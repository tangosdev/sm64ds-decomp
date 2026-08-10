//cpp
// @symbol _ZN11ChiefChilly8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * One frame of the boss. Almost everything here is gated on WHICH state is
 * current, compared by ADDRESS against twelve file-scope state objects, and it
 * gates five separate things rather than one:
 *
 *   three states       use UpdatePosWithOnlySpeed instead of UpdatePos, and
 *                      apply gravity by hand first: mVertSpeed becomes
 *                      max(mTerminalVelocity, mVertSpeed + mVertAccel)
 *   two states         at animation frame 7, build a world matrix from the
 *                      model's own bone and spawn the landing dust there --
 *                      the impact is tied to the ANIMATION, not to contact
 *   seven states       skip the ground ray entirely
 *   one more           is exempt from the rewind below
 *   two states         get an extra per-frame call
 *
 * THE GROUND RAY IS A FALL-THROUGH GUARD. It casts from 0x78000 above the boss
 * along its heading (reach doubles once unk_4cb > 1) and, when it finds
 * NOTHING, saves the current position into unk_4ec and rewinds mPos to the
 * triple at 0x068 -- so the boss cannot leave the arena by falling off it. A
 * hit clears unk_4c9; a miss while descending faster than 0xa000 sets it.
 *
 * The first thing it does is publish itself into the camera at +0x114, so the
 * camera follows the boss without the boss being asked.
 *
 * `#pragma opt_propagation off` is at FILE scope in the pre-image and is kept
 * there -- it is what stops the many position temporaries being folded.
 */
#pragma opt_propagation off
#include "ChiefChilly.h"
struct Mat4x3 { int m[12]; };

struct C;
typedef int (C::*PMF)();
struct C { char pad[0x37c]; PMF *pp; };

struct RayParams { Vector3 start, end, in, out; };

extern "C" {
extern void *data_0209f318;
extern Mat4x3 data_020a0e68;
extern Vector3 data_ov073_02123040;
extern char data_ov073_02123320[];
extern char data_ov073_02123330[];
extern char data_ov073_02123340[];
extern char data_ov073_02123350[];
extern char data_ov073_02123360[];
extern char data_ov073_02123380[];
extern char data_ov073_02123390[];
extern char data_ov073_021233a0[];
extern char data_ov073_021233c0[];
extern char data_ov073_021233d0[];
extern char data_ov073_021233f0[];
extern char data_ov073_02123400[];

extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *clsn);
extern int _ZNK9Animation12WillHitFrameEi(void *self, int f);
extern void MulMat4x3Mat4x3(void *d, void *a, void *b);
extern void Vec3_Lsl(Vector3 *d, Vector3 *s, int sh);
extern void func_02012694(int a, void *b);
extern void _ZN5Actor17HugeLandingDustAtER7Vector3b(void *self, Vector3 *v, int b);
extern void _ZN11RaycastLineC1Ev(void *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
extern void MulVec3Mat4x3(void *a, void *m, void *out);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, const Vector3 *a, const Vector3 *b, void *actor);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wmc, unsigned int flags);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
extern void func_ov073_0211f61c(void *self);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void func_ov073_021215cc(void *self);
extern void _ZN14BlendModelAnim7AdvanceEv(void *self);
}

int ChiefChilly::Behavior()
{
    char *self = (char *)this;
    C *c = (C *)this;
    int angx;
    Vector3 v0;
    RayParams rp;
    Vector3 v3C;
    Vector3 v48;
    Vector3 v54;
    int line[0x1f];

    *(C **)((char *)data_0209f318 + 0x114) = c;
    /* Enemy declares unk_100 as s16 -- 28 of its subclasses' generated headers
       say so -- while this file's own extern for the helper says u16*. Both are
       recovered guesses and nothing here settles which; the cast reproduces the
       ROM without asserting either. */
    DecIfAbove0_Short((unsigned short *)&unk_100);

    if (*(void **)((char *)c->pp + 8) != 0) {
        PMF *p = c->pp + 1;
        (c->**p)();
    }

    if ((char *)c->pp != data_ov073_02123400
        && (char *)c->pp != data_ov073_02123320
        && (char *)c->pp != data_ov073_02123340) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(self, &mMovingCylinderClsnWithPos);
    } else {
        int sum = mVertSpeed + mVertAccel;
        int m = mTerminalVelocity;
        int ac = unk_0ac;
        if (sum >= m) m = sum;
        mVertSpeed = m;
        unk_0ac = ac;
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(self, &mMovingCylinderClsnWithPos);
    }

    if (((char *)c->pp == data_ov073_02123330 || (char *)c->pp == data_ov073_02123350)
        && _ZNK9Animation12WillHitFrameEi(self + 0x35c, 7) != 0) {
        data_020a0e68 = *(Mat4x3 *)(self + 0x328);
        MulMat4x3Mat4x3(*(char **)(self + 0x320) + 0x60, &data_020a0e68, &data_020a0e68);
        v0.x = data_020a0e68.m[9];
        v0.y = data_020a0e68.m[10];
        v0.z = data_020a0e68.m[11];
        Vec3_Lsl(&v3C, &v0, 3);
        v0 = v3C;
        func_02012694(0x167, &mCamSpacePosX);
        v48 = v0;
        _ZN5Actor17HugeLandingDustAtER7Vector3b(self, &v48, 1);
    }

    if ((char *)c->pp != data_ov073_021233c0
        && (char *)c->pp != data_ov073_021233d0
        && (char *)c->pp != data_ov073_021233f0
        && (char *)c->pp != data_ov073_02123400
        && (char *)c->pp != data_ov073_02123320
        && (char *)c->pp != data_ov073_02123340
        && (char *)c->pp != data_ov073_02123380) {
        _ZN11RaycastLineC1Ev(line);
        rp.start.x = 0; rp.start.y = 0; rp.start.z = 0;
        rp.end.x = 0; rp.end.y = 0; rp.end.z = 0;
        rp.in.x = 0; rp.in.y = 0; rp.in.z = 0;
        rp.out.x = 0; rp.out.y = 0; rp.out.z = 0;
        {
            int y;
            rp.start.x = mPosX;
            angx = 0x2000;
            y = mPosY;
            rp.start.y = y;
            rp.start.z = mPosZ;
            rp.start.y = y + 0x78000;
            if (unk_4cb > 1)
                rp.in.z = 0x258000;
            else
                rp.in.z = 0x12c000;
            Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, angx);
        }
        MulVec3Mat4x3(&rp.in, &data_020a0e68, &rp.out);
        {
            int sx = rp.start.x;
            int ox = rp.out.x;
            int sy = rp.start.y;
            int sz = rp.start.z;
            int oy, oz;
            rp.end.x = sx;
            rp.end.x = sx + ox;
            oy = rp.out.y;
            oz = rp.out.z;
            rp.end.y = sy;
            rp.end.y = sy + oy;
            rp.end.z = sz;
            rp.end.z = sz + oz;
        }
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(line, &rp.start, &rp.end, self);
        if (_ZN11RaycastLine10DetectClsnEv(line) == 0) {
            if (mHorzSpeed > 0xa000) {
                unk_4c9 = 1;
            }
            unk_4ec = mPosX;
            unk_4f0 = mPosY;
            unk_4f4 = mPosZ;
            if ((char *)c->pp != data_ov073_021233a0) {
                mPosX = unk_068;
                mPosY = unk_06c;
                mPosZ = unk_070;
                mHorzSpeed = 0;
            }
        } else {
            unk_4c9 = 0;
        }
        _ZN11RaycastLineD1Ev(line);
    }

    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(self, &mWithMeshClsn, 0);

    v54 = data_ov073_02123040;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(&mMovingCylinderClsnWithPos, &v54);

    if ((char *)c->pp == data_ov073_02123360
        || (char *)c->pp == data_ov073_02123390) {
        func_ov073_0211f61c(self);
    }
    _ZN12CylinderClsn5ClearEv(&mMovingCylinderClsnWithPos);
    _ZN12CylinderClsn6UpdateEv(&mMovingCylinderClsnWithPos);
    func_ov073_021215cc(self);
    _ZN14BlendModelAnim7AdvanceEv(&mBlendModelAnim);
    return 1;
}
