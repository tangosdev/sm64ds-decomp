//cpp
// @symbol _ZN13daObjDorifu_c13InitResourcesEP20daObjDorifuResources
#include "daObjDorifu_c.h"

typedef struct { s32 words[12]; } Matrix4x3Copy;

extern "C" {
void Matrix4x3_FromRotationY(void *mat, s32 angle);

/* A genuine SetFile call with its by-value Fix12<int> parameter triggers known
 * mwccarm wall 6az and grows this function by eight bytes. Keep the ABI-exact
 * declaration until that compiler wall has a source-form solution. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mat, s32 scale, s16 angle, void *clps);
}

/* Shared initialization for the three concrete drifting-platform descendants.
 * The raw matrix copies are load-bearing: ordinary C++ struct assignment
 * scalarizes them instead of emitting the ROM's ldm/stm sequence. */
s32 daObjDorifu_c::InitResources(daObjDorifuResources *resources)
{
    s32 i;
    char *resourcePos;
    char *modelPos;
    s32 j;
    char *sourceMat;
    s32 scale = 0x1000;
    char *modelDst;
    char *clsnPos;
    char *self = (char *)this;
    char *resourceBytes = (char *)resources;

    resourcePos = resourceBytes;
    modelPos = self + 0x320;
    for (i = 0; i < 5; ++i) {
        ((Model *)modelPos)->SetFile(
            (BMD_File *)Model::LoadFile(**(SharedFilePtr **)resourcePos), 1, -1);
        resourcePos += 0xc;
        modelPos += 0x50;
    }

    mActivePlank = 4;
    mStepTimer = 2;
    mEventBit = param1 & 0xf;
    Matrix4x3_FromRotationY(self + 0x33c, mAngleY);
    *(s32 *)(self + 0x360) = mPosX >> 3;
    *(s32 *)(self + 0x364) = mPosY >> 3;
    *(s32 *)(self + 0x368) = mPosZ >> 3;
    *(Matrix4x3Copy *)&mPlatformMat = *(Matrix4x3Copy *)(self + 0x33c);
    *(s32 *)(self + 0xdbc) = mPosX;
    *(s32 *)(self + 0xdc0) = mPosY;
    *(s32 *)(self + 0xdc4) = mPosZ;

    j = 0;
    sourceMat = self + 0x33c;
    modelDst = self + 0x320;
    clsnPos = self + 0x4b0;
    for (; j < 5; ++j) {
        *(Matrix4x3Copy *)(modelDst + 0x1c) = *(Matrix4x3Copy *)sourceMat;
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            clsnPos,
            dBgW_Kc::LoadFile(**(SharedFilePtr **)(resourceBytes + 4)),
            &mPlatformMat, scale, mAngleY,
            *(void **)(resourceBytes + 8));
        if (j == mActivePlank)
            ((dBgW_KcMbg *)clsnPos)->Enable(this);
        modelDst += 0x50;
        resourceBytes += 0xc;
        clsnPos += 0x1c8;
    }
    return 1;
}
