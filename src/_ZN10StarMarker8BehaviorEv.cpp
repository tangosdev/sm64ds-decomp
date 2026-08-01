//cpp
// @symbol _ZN10StarMarker8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "StarMarker.h"
typedef struct Mtx { int m[12]; } Mtx;
extern "C" {
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, int rad, int h, unsigned int g);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN12CylinderClsn5ClearEv(void *p);
extern void _ZN12CylinderClsn6UpdateEv(void *p);

extern unsigned char data_0209f208;
extern unsigned char *data_0209f344;
extern Mtx data_02082128;
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
        Matrix4x3_FromRotationY(((char *)this) + 0x130, unk_08e);
        unk_154 = mPosX >> 3;
        unk_158 = mPosY >> 3;
        unk_15c = mPosZ >> 3;
    }
    if ((unsigned int)(mFlags << 0x1e) >> 0x1f) {
        *(Mtx *)((char *)&unk_18c) = data_02082128;
        unk_1b0 = mPosX >> 3;
        unk_1b4 = mPosY >> 3;
        unk_1b8 = mPosZ >> 3;
        {
            int d = mPosY - unk_1c8;
            int rad = 0xa0000;
            if (mState != 0)
                rad = 0xc8000;
            _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                ((char *)this), ((char *)this) + 0x164, ((char *)this) + 0x18c, rad, d + 0x28000, 0xf);
        }
    }
    if (mState != 0) {
        if ((unsigned int)(mFlags << 0x1e) >> 0x1f) {
            if (mState != 2 && unk_0f8 != 0) {
                char *a = _ZN5Actor10FindWithIDEj(unk_0f8);
                if (a != 0) {
                    if ((unk_0f4 & 0x408000) != 0) {
                        unk_1d0 = (int)a;
                        func_ov002_020e7d84(((char *)this));
                        return 1;
                    }
                }
            }
            _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsnWithPos);
            _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsnWithPos);
        }
    }
    return 1;
}
