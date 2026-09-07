//cpp
// @symbol _ZN10KoopaShell8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * One frame of the shell, in five stages: shared enemy pre-pass, Yoshi's
 * mouth, the despawn countdown, the state tick, then movement and terrain.
 *
 * The Yoshi branch is where mSpawnAngleY earns its name: when the shell is
 * spat back out, the heading it had is stashed there, the state is forced to
 * data_ov102_0214ea78, and flag 0x80000 is cleared.
 *
 * The despawn countdown only runs in ONE state (0214ea68). In any other state
 * the timer is reset to 0 rather than being left to run, so a shell that is
 * picked back up stops expiring.
 *
 * The state tick is a pointer-to-member read out of the state record at +0x8
 * and null-checked before the call -- a null tick reads as "keep going", and
 * any state returning 0 ends the frame early.
 *
 * Terrain only matters while mVertAccel is non-zero. Hitting a wall in the spat-out
 * state kills the shell (and returns 0, the one non-1 exit); landing in the
 * rolling state zeroes both speeds. Two other states are exempt from the
 * on-ground call entirely.
 */
#include "types.h"
#include "KoopaShell.h"

struct C {};
typedef int (C::*PMF)();
struct EState {
    void *pad0;
    void *pad4;
    PMF fn; /* +0x8 */
};

extern char data_ov102_0214ea48;
extern char data_ov102_0214ea58;
extern char data_ov102_0214ea68;
extern char data_ov102_0214ea78;

extern "C" {
extern int func_ov002_020ad660(char *, char *, char *, int);
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(char *, char *);
extern void func_ov102_0214d1f8(char *, void *);
extern void func_ov102_0214ce60(char *);
extern void _ZN5dCc_c5ClearEv(char *);
extern int DecIfAbove0_Byte(char *);
extern void _ZN7fBase_c18MarkForDestructionEv(char *);
extern int DecIfAbove0_Short(char *);
extern void func_ov102_0214cbec(char *);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char *, char *);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(char *, char *, u32);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(char *);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(char *);
extern void _ZN8dActor_c8PoofDustEv(char *);
extern void func_ov102_0214c7fc(char *);
extern void func_ov102_0214c84c(char *);
extern void _ZN5dCc_c6UpdateEv(char *);
}

int KoopaShell::Behavior()
{
    char *c = (char *)this;

    if (func_ov002_020ad660(c, (char *)&mMeshClsn, (char *)&mModel, 3) != 0)
        return 1;

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(c, (char *)&mMeshClsn) != 0) {
        if (mEatenByYoshi != 0) {
            mSpawnAngleY = mPrevAngleY;
            func_ov102_0214d1f8(c, &data_ov102_0214ea78);
            mFlags &= ~0x80000u;
            mEatenByYoshi = 0;
        }
        func_ov102_0214ce60(c);
        _ZN5dCc_c5ClearEv((char *)&mdCc_c);
        return 1;
    }

    if (mDespawnTimer != 0 &&
        mState == (void *)&data_ov102_0214ea68) {
        if (DecIfAbove0_Byte((char *)&mDespawnTimer) == 0) {
            _ZN7fBase_c18MarkForDestructionEv(c);
            return 1;
        }
    } else {
        mDespawnTimer = 0;
    }

    DecIfAbove0_Short((char *)&mStateTimer);

    {
        EState *st = (EState *)mState;
        int res;
        if (*(int *)&st->fn == 0)
            res = 1;
        else
            res = (((C *)c)->*st->fn)();
        if (res == 0)
            return 1;
    }

    func_ov102_0214cbec(c);

    if (mVertAccel != 0) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char *)&mdCc_c);
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, (char *)&mMeshClsn, 0);
        if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mMeshClsn) != 0 ||
            _ZNK10dBgCh_Actr8IsOnWallEv((char *)&mMeshClsn) != 0) {
            if (_ZNK10dBgCh_Actr8IsOnWallEv((char *)&mMeshClsn) != 0) {
                if (mState == (void *)&data_ov102_0214ea78) {
                    _ZN8dActor_c8PoofDustEv(c);
                    _ZN7fBase_c18MarkForDestructionEv(c);
                    return 0;
                }
            }
            if (_ZNK10dBgCh_Actr10IsOnGroundEv((char *)&mMeshClsn) != 0) {
                if (mState == (void *)&data_ov102_0214ea68) {
                    mVertAccel = 0;
                    mVertSpeed = 0;
                }
                if (mState != (void *)&data_ov102_0214ea48 &&
                    mState != (void *)&data_ov102_0214ea58)
                    func_ov102_0214c7fc(c);
            }
        }
    }

    if (mModelIndex == 0)
        func_ov102_0214c84c(c);
    func_ov102_0214ce60(c);
    _ZN5dCc_c5ClearEv((char *)&mdCc_c);
    if (mState != (void *)&data_ov102_0214ea48)
        _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
    return 1;
}
