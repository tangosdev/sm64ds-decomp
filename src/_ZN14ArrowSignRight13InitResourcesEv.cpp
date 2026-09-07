//cpp
#include "ArrowSignRight.h"

struct ArrowSignFileColumn {
    void *value;
    void *nextColumn1;
    void *nextColumn2;
};

extern "C" {
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
void func_ov098_02137c8c(char *self);
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int scale, short angle, void *clps);

/* Each row is model/KCL/CLPS, but the ROM gives each column its own symbol.
 * These three overlapping stride-0xc views keep those relocation destinations
 * distinct while still indexing the table as rows. */
extern ArrowSignFileColumn data_ov098_0213c380[];
extern ArrowSignFileColumn data_ov098_0213c384[];
extern ArrowSignFileColumn data_ov098_0213c388[];
}

int ArrowSignRight::InitResources()
{
    u16 id = actorID;
    if (id != 0x12b) {
        if (id == 0x12c)
            mVariant = 1;
    } else {
        mVariant = 0;
    }

    u32 modelIndex = mVariant;
    void *model = _ZN5Model8LoadFileER13SharedFilePtr(data_ov098_0213c380[modelIndex].value);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, model, 1, -1);
    mShadowModel.InitCuboid();
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(this);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(this);
    func_ov098_02137c8c((char *)this);

    u32 collisionIndex = mVariant;
    void *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov098_0213c384[collisionIndex].value);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        data_ov098_0213c388[collisionIndex].value);
    return 1;
}
