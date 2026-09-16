//cpp
#include "dBgCh_Actr.h"
#include "dActor_c.h"
#include "fBase_c.h"
#include "dCc_c.h"
#include "dEnemyBase_c.h"
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
extern void func_ov102_0214d1f8(char *, void *);
extern void func_ov102_0214ce60(char *);
extern int DecIfAbove0_Byte(char *);
extern int DecIfAbove0_Short(char *);
extern void func_ov102_0214cbec(char *);
extern void func_ov102_0214c7fc(char *);
extern void func_ov102_0214c84c(char *);
}

int KoopaShell::Behavior()
{
    char *c = (char *)this;

    if (func_ov002_020ad660(c, (char *)&mMeshClsn, (char *)&mModel, 3) != 0)
        return 1;

    if (((dEnemyBase_c *)c)->UpdateYoshiEat(*(dBgCh_Actr *)&mMeshClsn) != 0) {
        if (mEatenByYoshi != 0) {
            mSpawnAngleY = mPrevAngleY;
            func_ov102_0214d1f8(c, &data_ov102_0214ea78);
            mFlags &= ~0x80000u;
            mEatenByYoshi = 0;
        }
        func_ov102_0214ce60(c);
        ((dCc_c *)&mdCc_c)->Clear();
        return 1;
    }

    if (mDespawnTimer != 0 &&
        mState == (void *)&data_ov102_0214ea68) {
        if (DecIfAbove0_Byte((char *)&mDespawnTimer) == 0) {
            ((fBase_c *)c)->MarkForDestruction();
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
        ((dActor_c *)c)->UpdatePos((dCc_c *)&mdCc_c);
        ((dEnemyBase_c *)c)->UpdateWMClsn(*(dBgCh_Actr *)&mMeshClsn, 0);
        if (((dBgCh_Actr *)&mMeshClsn)->IsOnGround() != 0 ||
            ((dBgCh_Actr *)&mMeshClsn)->IsOnWall() != 0) {
            if (((dBgCh_Actr *)&mMeshClsn)->IsOnWall() != 0) {
                if (mState == (void *)&data_ov102_0214ea78) {
                    ((dActor_c *)c)->PoofDust();
                    ((fBase_c *)c)->MarkForDestruction();
                    return 0;
                }
            }
            if (((dBgCh_Actr *)&mMeshClsn)->IsOnGround() != 0) {
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
    ((dCc_c *)&mdCc_c)->Clear();
    if (mState != (void *)&data_ov102_0214ea48)
        ((dCc_c *)&mdCc_c)->Update();
    return 1;
}
