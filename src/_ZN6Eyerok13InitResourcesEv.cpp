//cpp
#include "types.h"
// @symbol _ZN6Eyerok13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Eyerok.h"
#include "TextureSequence.h"
#include "dBgW.h"
extern int data_ov066_0211ae6c[];
extern int data_ov066_0211ae4c[];
extern int data_ov066_0211aeb4[];
extern int data_ov066_0211aebc[];
extern int data_ov066_0211ae9c[];
extern int data_ov066_0211ae3c[];
extern int data_ov066_0211ae2c[];
extern int data_ov066_0211ae5c[];
extern int data_ov066_0211ae84[];
extern int data_ov066_0211aea4[];
extern int data_ov066_0211ae8c[];
extern int data_ov066_0211ae54[];
extern int data_ov066_0211ae94[];
extern int data_ov066_0211ae64[];
extern int data_ov066_0211ae44[];
extern int data_ov066_0211ae74[];
extern int data_ov066_0211ae7c[];
extern int data_ov066_0211ae24[];
extern int data_ov066_0211aeac[];
extern int data_ov066_0211ae14[];
extern int data_ov066_0211ae1c[];
extern int data_ov066_0211ae34[];
extern s8 data_ov066_0211abe0;
extern s8 data_ov066_0211ae04;
extern s8 data_ov066_0211ae0c;
extern "C" char _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

/* extern "C" is load-bearing on every mangled/C-named declaration below: without
   it a .cpp file mangles the name a SECOND time (e.g.
   _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as ->
   _Z44_ZN8dActor_c5Spawn...jjP7Vector3Pvii, func_020393d4 -> _Z13func_020393d4PvS_),
   which exists nowhere -- and the file byte-matches either way (match.py compares
   relocated words as wildcards), so only check_references sees it. */
extern "C" u8 _ZN8dActor_c9TrackStarEjj(void* actor, u32 a, u32 b);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern "C" void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* sfp);
extern "C" void _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern "C" void _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* sfp);
extern "C" void _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* thiz, void* actor, Vector3* v, s32 f1, s32 f2, u32 a, u32 b);
extern "C" void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 b, Vector3* pos, void* p, int e, int f);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, int kcl, void* mtx, s32 fix, s16 s, void* clps);
extern "C" void func_020393d4(void* p, void* v);
extern "C" void func_020393c4(void* p, void* v);

int Eyerok::InitResources()
{
    char* c = (char*)((void*)this);
    Vector3 v;
    Vector3 w;

    mPartIdx = (s32)param1 & 0xFF;
    if (mPartIdx == 0xFF)
        mPartIdx = 0;
    mStarId = (param1 >> 0xC) & 0xF;
    mStarTracked = _ZN8dActor_c9TrackStarEjj(c, mStarId, 2);
    if (mPartIdx > 2)
        mPartIdx = 0;

    switch (mPartIdx) {
    case 0:
        _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel2, _ZN5Model8LoadFileER13SharedFilePtr(data_ov066_0211ae6c), 1, -1);
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov066_0211ae4c);
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov066_0211aeb4);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211aebc);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211ae9c);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211ae3c);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211ae2c);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae5c);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae84);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211aea4);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae8c);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae54);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae94);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae64);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae44);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae74);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae7c);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211ae24);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211aeac);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211ae14);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211ae1c);
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov066_0211ae34);
        break;
    case 1:
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mBlendModelAnim, (void*)data_ov066_0211ae4c[1], 1, -1) == 0)
            return 0;
        TextureSequence::Prepare(*(BMD_File*)data_ov066_0211ae4c[1], *(BTP_File*)data_ov066_0211aebc[1]);
        TextureSequence::Prepare(*(BMD_File*)data_ov066_0211ae4c[1], *(BTP_File*)data_ov066_0211ae9c[1]);
        break;
    case 2:
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(&mBlendModelAnim, (void*)data_ov066_0211aeb4[1], 1, -1) == 0)
            return 0;
        TextureSequence::Prepare(*(BMD_File*)data_ov066_0211aeb4[1], *(BTP_File*)data_ov066_0211ae3c[1]);
        TextureSequence::Prepare(*(BMD_File*)data_ov066_0211aeb4[1], *(BTP_File*)data_ov066_0211ae2c[1]);
        break;
    }

    if (mPartIdx != 0) {
        _ZN11ShadowModel12InitCylinderEv(&mShadowModel);
        w.x = data_ov066_0211ad18[0];
        w.y = data_ov066_0211ad18[1];
        w.z = data_ov066_0211ad18[2];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, c, &w, 0x64000, 0x64000, 0x200002, 0);
    }

    {
        /* NOT mDustPos[i]: the ROM walks a running char* and re-derives the
           three stores from it. Spelling this as `Vector3 *p = mDustPos; p->x
           = 0; ... p += 1;` costs the function its size -- measured. */
        int i = 0;
        char* p = c;
        do {
            *(s32*)(p + 0x4DC) = 0;
            *(s32*)(p + 0x4E0) = 0;
            i += 1;
            *(s32*)(p + 0x4E4) = 0;
            p += 0xC;
        } while (i < 0x14);
    }

    mTerminalVelocity = -0x64000;
    mHandUniqueID1 = 0;
    mHandUniqueID2 = 0;

    if (mPartIdx == 0) {
        void* r;
        mPosZ -= 0x7C000;
        mRestPosX = mPosX;
        mRestPosY = mPosY;
        mRestPosZ = mPosZ;
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.x += 0x193000;
        r = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xB0, 1, &v, 0, mAreaId, -1);
        if (r != 0)
            mHandUniqueID1 = *(s32*)((char*)r + 4);
        v.x = mPosX;
        v.x -= 0x18C000;
        r = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xB0, 2, &v, 0, mAreaId, -1);
        if (r != 0)
            mHandUniqueID2 = *(s32*)((char*)r + 4);
        data_ov066_0211ae10 = 0;
        data_ov066_0211ae08 = 0;
        data_ov066_0211ae0c = 0;
        data_ov066_0211abe4 = 1;
        data_ov066_0211ae04 = 1;
        data_ov066_0211abe0 = 3;
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider2, data_ov066_0211ae24[1], &mClsnMat2, 0x199, mAngleY, &func_02112ca8);
        func_020393d4(&mMeshCollider2, &_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
        func_020393c4(&mMeshCollider2, &func_ov066_0211a35c);
        ((dBgW *)&mMeshCollider2)->Enable(this);
        mTimer2 = 0x64;
        func_ov066_02119454(c, data_ov066_0211b09c);
    } else {
        mRestPosX = mPosX;
        mRestPosY = mPosY;
        mRestPosZ = mPosZ;
        mSpawnPosX = mPosX;
        mSpawnPosY = mPosY;
        mSpawnPosZ = mPosZ;
        if (mPartIdx == 1) {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider2, data_ov066_0211ae14[1], &mClsnMat2, 0x199, mAngleY, &func_02112c08);
            mRestPosX -= 0x31F000;
        } else {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(&mMeshCollider2, data_ov066_0211aeac[1], &mClsnMat2, 0x199, mAngleY, &func_02112d48);
            mRestPosX += 0x31F000;
        }
        func_020393d4(&mMeshCollider2, &_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
        func_020393c4(&mMeshCollider2, &func_ov066_0211a35c);
        func_020398fc(&mMeshCollider2);
        mRestPosZ -= 0x32000;
        unk_4d8 = 3;
        data_ov066_0211ae00 = 0;
        func_ov066_02119454(c, data_ov066_0211b05c);
    }
    return 1;
}
