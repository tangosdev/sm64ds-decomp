//cpp
// @symbol _ZN12FortressWall13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "FortressWall.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *kcl, void *mtx, int f, short s, void *blk);
}
struct E12 { void *p; int pad[2]; };
extern struct E12 data_ov079_02128058[];
extern struct E12 data_ov079_0212805c[];
extern struct E12 data_ov079_02128060[];

int FortressWall::InitResources()
{
    int b = (int)(actorID == 0x30);
    if (b != 0) {
        mVariant = 0;
        mStarId = (unsigned char)(*(s32 *)&param1);
        if (mStarId == 0xff)
            mStarId = 0;
    } else {
        mVariant = 1;
    }
    {
        int idx = mVariant;
        void *mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov079_02128058[idx].p);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, mdl, 1, -1);
        _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
        {
            int idx2 = mVariant;
            void *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov079_0212805c[idx2].p);
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, data_ov079_02128060[idx2].p);
        }
    }
    return 1;
}
