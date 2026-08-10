//cpp
// @symbol _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj
/* recovered: named members + shared header, real C++ method
 *
 * One frame of the death an invincible (mega) character inflicts.
 * Enemy::KillByInvincibleChar -- ov002 0x020ada40, still named
 * func_ov002_020ada40 in this tree -- starts it by writing mDeathState = 8 and
 * mDeathTimer = 0x1e; this runs only in that state and ends it either when the
 * timer expires or when the corpse lands (on ground while falling).
 *
 * While it runs, the enemy tumbles: the spin rates KillByInvincibleChar copied
 * out of its `const Vector3_16 &` argument are added to mAngleX/Y/Z once a
 * frame, and the model matrix is rebuilt around the body's own half-height so
 * it spins about its middle rather than its feet.
 *
 * `flags` is a two-bit selector, not a boolean: bit 0 drops a coin, bit 1
 * books the kill in the death table.
 *
 * Both reference parameters are null-checked. That is not a contradiction --
 * the compiler emits the test rather than folding it away, so `&ref == 0` is
 * how the ROM's check is spelled (see the note in include/Actor.h). Measured
 * under this family's pin, 2004/b56.
 */
#include "Enemy.h"

extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *clsn);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *clsn);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *actor);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *actor, void *clsn);
/* ReflectAngle takes Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with scalars in those slots. */
extern short _ZN5Actor12ReflectAngleE5Fix12IiES1_s(void *actor, int a, int b, short s);
extern void Vec3_Asr(void *dst, const void *src, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
extern char data_020a0e68;
}

/* Enemy.h is a flat struct with no base, so the class carries no vtable to call
   through; this stand-in exists only to reach one slot, and goes away when
   Enemy is rebased on Actor.

   WHAT that slot is, is left open on purpose. The ROM loads vtable+0x74 -- slot
   29, since Actor.h pins slot 20 at vtable+0x50 -- and uses the returned int,
   arithmetic-shifted right by 3, as the Y of a translation applied before the
   rotation and negated after it. That is the height of the point the model
   spins about, and nothing more specific is evidenced here.

   Actor.h labels slot 29 `OnAimedAtWithEgg()`, which does not fit that use. The
   name reached the tree from the dScMgBase_c / dScMgTrampoline2_c scene family
   ("recovered from vtable slot identity"), which is a different hierarchy, so
   it may simply not transfer to Actor. Flagged, not resolved -- renaming an
   Actor vtable slot is its own slice, and guessing one here would be exactly
   the un-evidenced assertion this file is trying to avoid. */
struct VB {
    virtual void d00();
    virtual void d01();
    virtual void d02();
    virtual void d03();
    virtual void d04();
    virtual void d05();
    virtual void d06();
    virtual void d07();
    virtual void d08();
    virtual void d09();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void d18();
    virtual void d19();
    virtual void d20();
    virtual void d21();
    virtual void d22();
    virtual void d23();
    virtual void d24();
    virtual void d25();
    virtual void d26();
    virtual void d27();
    virtual void d28();
    virtual int m29();
};

struct M48 { int w[12]; };

#define LAUNDER(p) ((int)(p))

int Enemy::UpdateKillByInvincibleChar(WithMeshClsn & ww_, ModelAnim & mm_, unsigned int flags)
{
    WithMeshClsn *clsn = &ww_;
    ModelAnim *anim = &mm_;
    int v[3];

    if (mDeathState != 8)
        return 0;

    if (mDeathTimer != 0)
        *(unsigned short *)LAUNDER(&mDeathTimer) -= 1;

    if (mDeathTimer == 0 ||
        (clsn != 0 && _ZNK12WithMeshClsn10IsOnGroundEv(clsn) != 0 && mVertSpeed < 0)) {
        if (flags & 1)
            SpawnCoin();
        if (flags & 2)
            _ZN5Actor24KillAndTrackInDeathTableEv(this);
        mDeathState = 0;
        return 2;
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(this, 0);
    if (clsn != 0) {
        UpdateWMClsn(*clsn, 0);
        if (_ZNK12WithMeshClsn8IsOnWallEv(clsn) != 0)
            mPrevAngleY = _ZN5Actor12ReflectAngleE5Fix12IiES1_s(
                this, mWallNormalX, mWallNormalZ, mPrevAngleY);
    }

    if (anim != 0) {
        *(short *)LAUNDER(&mAngleX) += mSpinRateX;
        *(short *)LAUNDER(&mAngleY) += mSpinRateY;
        *(short *)LAUNDER(&mAngleZ) += mSpinRateZ;
        Vec3_Asr(v, &mPosX, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            ((VB *)(void *)this)->m29() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
            mAngleX, mAngleY, mAngleZ);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0,
            (-((VB *)(void *)this)->m29()) >> 3, 0);
        *(M48 *)((char *)anim + 0x1c) = *(M48 *)&data_020a0e68;
    }
    return 1;
}
