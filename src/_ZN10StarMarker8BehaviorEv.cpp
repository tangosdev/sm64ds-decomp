//cpp
// @symbol _ZN10StarMarker8BehaviorEv
/* recovered: named members + shared header, real C++ method */
/* The class header comes FIRST on purpose: it reaches math/Matrix.h, whose
   Matrix4x3 is the structured one, and include/common.h's flat spelling stands
   down behind the guard. mat4x3.t is only nameable this way round, and the two
   spellings are the same 0x30 bytes. */
#include "StarMarker.h"
/* recovered: declarations from a shared header */
#include "decl_common.h"
typedef struct Mtx { int m[12]; } Mtx;
extern "C" {
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, int rad, int h, unsigned int g);
extern char *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN5dCc_c5ClearEv(void *p);
extern void _ZN5dCc_c6UpdateEv(void *p);

extern unsigned char data_0209f208;
extern unsigned char *data_0209f344;
extern Mtx IDENTITY_MATRIX4X3;
}

int StarMarker::Behavior()
{
    if ((unsigned int)(mFlags << 0x1c) >> 0x1f) {
        if (mStarID == data_0209f344[data_0209f208]) {
            mAppearTimer = 0;
            if (((unsigned int)(mFlags << 0x1f) >> 0x1f) == 0) {
                *(unsigned char *)((((int)((char *)this)) + 0x1db)) |= 2;
                *(int *)((((int)((char *)this)) + 0xec)) &= ~1;
            }
        } else {
            mAppearTimer = 0x2a;
        }
        *(unsigned char *)((((int)((char *)this)) + 0x1db)) &= ~8;
    }
    if (mState != 0) {
        if (mAppearTimer != 0) {
            if (((unsigned int)(mFlags << 0x1e) >> 0x1f) == 0) {
                if (mStarID == data_0209f344[data_0209f208]) {
                    *(unsigned short *)((((int)((char *)this)) + 0x1d4)) -= 1;
                    if (mAppearTimer == 0) {
                        if (((unsigned int)(mFlags << 0x1f) >> 0x1f) == 0) {
                            *(unsigned char *)((((int)((char *)this)) + 0x1db)) |= 2;
                            *(int *)((((int)((char *)this)) + 0xec)) &= ~1;
                        }
                    }
                }
            }
        }
        Matrix4x3_FromTranslation(((char *)this) + 0x130, mPosX >> 3, mPosY >> 3,
                                  mPosZ >> 3);
    } else {
        *(short *)((((int)((char *)this)) + 0x8e)) += 0x400;
        Matrix4x3_FromRotationY(((char *)this) + 0x130, mAngleY);
        mModel.mat4x3.t.x = mPosX >> 3;
        mModel.mat4x3.t.y = mPosY >> 3;
        mModel.mat4x3.t.z = mPosZ >> 3;
    }
    if ((unsigned int)(mFlags << 0x1e) >> 0x1f) {
        *(Mtx *)((char *)&mShadowMtx) = IDENTITY_MATRIX4X3;
        mShadowMtx.t.x = mPosX >> 3;
        mShadowMtx.t.y = mPosY >> 3;
        mShadowMtx.t.z = mPosZ >> 3;
        {
            int d = mPosY - mGroundY;
            int rad = 0xa0000;
            if (mState != 0)
                rad = 0xc8000;
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                ((char *)this), ((char *)this) + 0x164, ((char *)this) + 0x18c, rad, d + 0x28000, 0xf);
        }
    }
    if (mState != 0) {
        if ((unsigned int)(mFlags << 0x1e) >> 0x1f) {
            /* Both are fields of the dCcAcPos_c at 0x0d4, which the cartridge's own
               ~StarMarker names (tools/dtor_members.py): 0x0f8 is +0x24,
               dCc_c::otherOwner, and 0x0f4 is +0x20, dCc_c::hitFlags. */
            if (mState != 2 && mdCcAcPos_c.otherOwner != 0) {
                char *a = _ZN8dActor_c10FindWithIDEj(mdCcAcPos_c.otherOwner);
                if (a != 0) {
                    if ((mdCcAcPos_c.hitFlags & 0x408000) != 0) {
                        mHitActor = (dActor_c *)a;
                        Collect();
                        return 1;
                    }
                }
            }
            _ZN5dCc_c5ClearEv((char *)&mdCcAcPos_c);
            _ZN5dCc_c6UpdateEv((char *)&mdCcAcPos_c);
        }
    }
    return 1;
}
