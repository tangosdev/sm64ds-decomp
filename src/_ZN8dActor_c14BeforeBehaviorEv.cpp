//cpp
/* dActor_c::BeforeBehavior() at 0x02010fd4, 0x240 bytes -- vtable slot 7.
 *
 * The per-frame gate in front of every actor's Behavior. It answers "should this
 * actor think this frame", and on the way there it does the two pieces of
 * bookkeeping that have to happen whether it thinks or not: it projects the
 * actor into camera space, and it snapshots the position.
 *
 * The order matters and is the whole function:
 *
 *   1. Ask the base first; if it says no, nothing else runs.
 *   2. Clear 0x0209b458 -- the nearest-player cache that ClosestPlayer fills in
 *      (see the player-proximity note in include/dActor_c.h). It is a per-frame
 *      cache, so the frame starts by invalidating it.
 *   3. If the actor is bound to an area (mAreaId >= 0) that is not the showing
 *      one, force it off screen: set 0x38, push camera-space position to
 *      INT_MAX so nothing can consider it near, and stop.
 *   4. Otherwise project into camera space -- but only when mClipRadius, the clip
 *      radius, is nonzero. A zero radius means "no clip volume", and the
 *      transform is skipped entirely rather than computed and ignored.
 *   5. Run the clipper and set/clear the visibility bits from its answer.
 *   6. Snapshot mPos into mPrevPosX..mPrevPosZ so the next frame can see where the
 *      actor was.
 *
 * FLAG BITS, as this function uses them:
 *   0x00002  do not render while off screen
 *   0x00008  off screen        )  0x38 = these three, "fully invisible"
 *   0x00010  far away          )
 *   0x00020  wrong area        )
 *   0x00003  has a clip volume -- 0x10003 with the suppress bit
 *   0x10000  suppress behaviour; combined with off-screen it returns 0
 *
 * mAreaId is signed and negative means "not area-bound", which is why step 3
 * tests `>= 0` before calling IsAreaShowing rather than after.
 *
 * MIGRATION NOTE. Every offset this function used to reach through a bare
 * `char *self` is already named in include/dActor_c.h -- 0x5c/0x60/0x64 mPos,
 * 0x68..0x70 the previous-position snapshot, 0x74..0x7c mCamSpacePos, 0xb0
 * mFlags, 0xb4/0xb8/0xbc/0xc0/0xc4 the clip block, 0xcc mAreaId. Nothing here
 * needed new evidence; the names existed and this file was not using them.
 *
 * FOUR POINTER-THROUGH-`int` CASTS WERE REMOVED, AND THEY WERE NOT DOING
 * ANYTHING. The old source wrote every read-modify-write of the flags as
 * `*(u32*)((int)(self + 0xb0)) |= ...`, routing the address through `int` --
 * the shape you reach for when CW has to re-read a field instead of keeping it
 * in a register across the update. Both spellings were built and compared under
 * 2004/b56, and the plain `mFlags |= ...` is byte-identical across all 0x240
 * bytes. The casts bought nothing here, so they are gone rather than carried
 * across on the assumption that they must have been there for a reason.
 *
 * Clipper::Func_020150E8's mangled name carries a by-value Fix12<int>, so it
 * stays spelled out with scalar arguments -- see notes/mwccarm-codegen.md 6az.
 */
#include "dActor_c.h"

struct Matrix4x3 { s32 m[12]; };

extern "C" {

u8   IsAreaShowing(int areaId);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
int  _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(char *thisp, Vector3 *v,
                                                    int radius, u8 *out);

/* `extern` on every one of these, and it is load-bearing. Inside
   `extern "C" { ... }` a variable declaration WITHOUT `extern` is a
   DEFINITION in C++ -- C's tentative-definition rule does not apply -- so the
   object would define these nine alongside the ROM's gap object. The same
   shape in dActor_c::HorzAngleToFPlayer produced
   `Multiply-defined: "data_0209b450" ... Previously defined in
   _dsd_gap@main_40.o` and aborted the link. It did not abort here, which is
   luck about how mwcc emits each type, not correctness. Function declarations
   above need no `extern`; only the objects do. */
extern u32       data_0209b458;   /* nearest-player cache, cleared each frame */
extern int       data_0209fc68;   /* nonzero forces the actor to think anyway */
extern Matrix4x3 data_0209b3ec;   /* world -> camera */
extern char      data_0209f43c;   /* the Clipper instance */
extern u8        data_0209f274;   /* doubles the far-away threshold when set */
extern u8        data_0209f2c4;
extern u8        data_0209f20c;
extern u8        data_0209f294;
extern u32       data_0209b464;   /* flag mask that forces thinking; 0 means "any" */

}

int dActor_c::BeforeBehavior()
{
    if (!fBase_c::BeforeBehavior())
        return 0;

    data_0209b458 = 0;

    signed char c = mAreaId;
    if (c >= 0 && !IsAreaShowing(c)) {
        mFlags |= 0x38;
        mCamSpacePosX = 0x7fffffff;
        mCamSpacePosY = 0x7fffffff;
        mCamSpacePosZ = 0x7fffffff;
        if (data_0209fc68 == 0 || (mFlags & 0x10000))
            return 0;
    } else {
        if (mClipRadius != 0) {
            Vector3 tmp;
            tmp.x = mPosX >> 3;
            tmp.y = (mPosY + mClipOffsetY) >> 3;
            tmp.z = mPosZ >> 3;
            MulVec3Mat4x3(&tmp, &data_0209b3ec, (Vector3 *)&mCamSpacePosX);
        }
        if (mFlags & 0x10003) {
            int r = _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(
                        &data_0209f43c,
                        (Vector3 *)&mCamSpacePosX,
                        mClipRadius,
                        &mClipResult);
            int thresh = mClipDistance;
            if (data_0209f274)
                thresh <<= 1;
            if (r > thresh) {
                mFlags |= 0x18;
                if (mFlags & 0x10000)
                    return 0;
            } else {
                u32 *p = &mFlags;
                *p &= ~0x38;
                if (r > mFarDistance)
                    *p |= 0x10;
            }
        } else {
            mFlags &= ~0x38;
        }
    }

    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) == 0) {
        u32 f = mFlags;
        if ((f & 9) != 9 && (data_0209b464 == 0 || (f & data_0209b464) != 0))
            goto do_copy;
        if (data_0209fc68 != 0)
            goto do_copy;
    }
    return 0;

do_copy:
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    return 1;
}
