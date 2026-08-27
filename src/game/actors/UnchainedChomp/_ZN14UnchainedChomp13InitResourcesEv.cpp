//cpp
// @symbol _ZN14UnchainedChomp13InitResourcesEv

#include "SharedFilePtr.h"
#include "UnchainedChomp.h"

struct PathPtr {
    char pad_00[8];

    PathPtr();
    void FromID(u32 id);
    u32 NumNodes() const;
    void GetNode(Vector3 &out, u32 index) const;
};

extern "C" {
void LoadSilverStarAndNumber();
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *clsn, dActor_c *actor, const Vector3 &offset,
    s32 radius, s32 height, u32 flags, u32 vulnFlags);
void func_ov100_02143b18(UnchainedChomp *chomp, void *data);

extern SharedFilePtr data_ov002_0211092c;
extern SharedFilePtr data_ov100_021486bc;
extern SharedFilePtr data_ov100_021486a4;
extern SharedFilePtr data_ov100_021486ac;
extern SharedFilePtr data_ov100_021486b4;
extern s32 data_ov100_02148008[3];
extern s32 data_ov100_021486f4;
}

int UnchainedChomp::InitResources()
{
    Model::LoadFile(data_ov002_0211092c);
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov100_021486bc), 1, -1);
    Model::LoadFile(data_ov100_021486a4);
    Animation::LoadFile(data_ov100_021486ac);
    Animation::LoadFile(data_ov100_021486b4);
    LoadSilverStarAndNumber();

    {
        int i = 0;
        Model *model = mModels;
        do {
            model->SetFile(*(BMD_File **)((char *)&data_ov100_021486a4 + 4), 1, -1);
            i++;
            model++;
        } while (i < 6);
    }

    mShadowModel.InitCylinder();
    {
        int i = 0;
        ShadowModel *shadow = mShadowModels;
        do {
            shadow->InitCylinder();
            i++;
            shadow++;
        } while (i < 6);
    }

    unk_6ac = param1 & 0xff;
    unk_6b8 = (param1 >> 8) & 0xf;
    if (unk_6ac == 0xff)
        unk_6ac = 0;

    {
        PathPtr path;
        path.FromID(unk_6ac);
        unk_6b0 = path.NumNodes();
    }

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;

    {
        Vector3 offset;
        offset.x = data_ov100_02148008[0];
        offset.y = data_ov100_02148008[1];
        offset.z = data_ov100_02148008[2];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mdCcAcPos_c, this, offset,
            0xaa000, 0x140000, 0x200004, 0x6010);
    }

    unk_6c9 = 0x1f;
    unk_6cc = 3;

    mAngleY = mPrevAngleY;
    *(s16 *)((char *)this + 0x6a4) = mAngleY;

    unk_6d0 = 0;
    unk_6d4 = 0;

    {
        int i = 0;
        char *position = (char *)this;
        do {
            *(s32 *)(position + 0x6d8) = mPosX;
            i++;
            *(s32 *)(position + 0x6dc) = mPosY;
            *(s32 *)(position + 0x6e0) = mPosZ;
            position += sizeof(Vector3);
        } while (i < 6);
    }

    *(s16 *)((char *)this + 0x6ca) = 0xc8;

    {
        PathPtr path;
        path.FromID(unk_6ac);
        unk_6b4 = 1;
        path.GetNode(*(Vector3 *)&mPosX, unk_6b4);
    }

    /* Preserve the ROM's materialized read-modify-write address for mPosY. */
    *(s32 *)((int)this + 0x60) += 0x64000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    func_ov100_02143b18(this, &data_ov100_021486f4);
    return 1;
}
