//cpp
// @symbol _ZN7Chuckya8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * One frame of Chuckya. The state tick runs first; everything after it is
 * gated on WHICH state is current, compared by address against seven
 * file-scope state objects.
 *
 * Three separate gates, not one:
 *   0211dea0                       skips terrain entirely
 *   0211dec0 / 0211de70            run terrain but skip the cliff check
 *   0211ded0/dee0/de90/df00        additionally call func_ov062_02116010
 *
 * The cliff check is how Chuckya refuses to walk off an edge, and it rewinds
 * rather than stops: on a trip it zeroes mHorzSpeed, sets unk_3e4, and restores
 * mPos from mPrevPos -- last frame's position, republished at the end of every
 * frame that runs terrain. Two hard bounds sit alongside it, selected by the
 * level in data_0209f2f8 (0x16 tests X, 0x15 tests Z), so a level can fence
 * Chuckya in where the mesh alone would not.
 *
 * The angle triple at 0x92 is copied wholesale into mAngleX/Y/Z right after
 * the tick, so the state writes there and this publishes it -- see Chuckya.h
 * for why those three keep unk_ names.
 *
 * The early exit is the throw: when something is held, flag 0x4000 is set and
 * the held object's +0xc8 is non-zero, func_ov062_02116d28 runs and the frame
 * ends before the cylinder update and the three zeroed words. Chuckya::Render
 * asks the identical triple.
 */
#include "Chuckya.h"

struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;

extern "C" {
int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void* self, void* wm, void* anim, unsigned int n);
unsigned short DecIfAbove0_Short(unsigned short* p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, CylinderClsn* cc);
int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(void* self, WithMeshClsn* wm, int a, s16 b, int c, int d, void* e);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* self, WithMeshClsn* wm, unsigned int j);
void func_ov062_02116010(void* self);
void _ZN12CylinderClsn5ClearEv(CylinderClsn* self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn* self);
void _ZN9Animation7AdvanceEv(void* self);
void func_ov062_02116d28(char* c);
void func_ov062_02116e80(void* c);
void func_ov062_02116dbc(char* c);
extern signed char data_0209f2f8;
extern char data_ov062_0211dea0[];
extern char data_ov062_0211dec0[];
extern char data_ov062_0211de70[];
extern char data_ov062_0211ded0[];
extern char data_ov062_0211dee0[];
extern char data_ov062_0211de90[];
extern char data_ov062_0211df00[];
}

int Chuckya::Behavior()
{
    char* c = (char*)this;

    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, &mMeshClsn, &mModel, 3))
        return 1;

    DecIfAbove0_Short((unsigned short *)&unk_100);
    DecIfAbove0_Short(&unk_3e6);
    DecIfAbove0_Short(&unk_3e8);

    {
        M* m = (M*)mState;
        if (m->pmf != 0)
            (((Klass*)c)->*(m->pmf))();
    }

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (CylinderClsn*)&mCylinderClsn);

    if (mState != (void*)data_ov062_0211dea0) {
        if (mState != (void*)data_ov062_0211dec0
            && mState != (void*)data_ov062_0211de70) {
            int r2 = 0;
            signed char t = data_0209f2f8;
            if (t == 0x16) {
                if (mPosX > (int)0xff95c000)
                    r2 = 1;
            } else if (t == 0x15) {
                if (mPosZ < (int)0xff2f4000)
                    r2 = 1;
            }
            if (r2 != 0
                || (mHorzSpeed != 0
                    && _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(
                           c, (WithMeshClsn*)&mMeshClsn, 0x3c000, (s16)0x2888, 0, 1, (void*)0x32000))) {
                mHorzSpeed = 0;
                unk_3e4 = 1;
                mPosX = mPrevPosX;
                mPosY = mPrevPosY;
                mPosZ = mPrevPosZ;
            } else {
                if (unk_3e4 == 1)
                    unk_3e4 = 0;
            }
        }
        mPrevPosX = mPosX;
        mPrevPosY = mPosY;
        mPrevPosZ = mPosZ;
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, (WithMeshClsn*)&mMeshClsn, 3);
    }

    if (mState == (void*)data_ov062_0211ded0
        || mState == (void*)data_ov062_0211dee0
        || mState == (void*)data_ov062_0211de90
        || mState == (void*)data_ov062_0211df00) {
        func_ov062_02116010(c);
    }

    _ZN12CylinderClsn5ClearEv((CylinderClsn*)&mCylinderClsn);
    mModel.Advance();

    {
        char* o = (char*)(((int)c + 0x300));
        (*(void(**)(void*))(*(int*)o + 0xc))(o);
    }

    {
        char* p3f8 = (char*)mHeld;
        if (p3f8 != 0) {
            int flag = (mFlags & 0x4000) != 0;
            if (flag) {
                if (*(int*)(p3f8 + 0xc8) != 0) {
                    func_ov062_02116d28(c);
                    goto ret;
                }
            }
        }
    }

    _ZN12CylinderClsn6UpdateEv((CylinderClsn*)&mCylinderClsn);
    unk_42c = 0;
    unk_430 = 0;
    unk_434 = 0;
    func_ov062_02116e80(c);
    func_ov062_02116dbc(c);

ret:
    return 1;
}
