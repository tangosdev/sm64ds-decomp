//cpp
// @symbol _ZN10daPgDfdr_c13InitResourcesEv
/* daPgDfdr_c::InitResources -- vtable slot 0. Real C++ method over the shared
 * header. Was an extern "C" free function over a raw `char *` with every field
 * reached by literal offset; converting it and naming the members is byte-exact
 * under the pinned 2004/b56.
 *
 * It loads the model into mModelAnim, its three animations and its BTP texture
 * sequence, hard-codes the object's world position and facing, hangs the KCL on
 * the mesh collider with dBgW::UpdatePosAndAngs as the update hook, disables
 * gravity (mVertAccel and mTerminalVelocity both zero), starts the actor
 * collider, and finally drops the object onto whatever ground a raycast 0x14000
 * above it finds -- falling back to that raised point when nothing is hit.
 *
 * dBgW_KcMbg::SetFile, TextureSequence::SetFile and dCcAc_c::Init all carry
 * Fix12<int> BY VALUE in their mangled names (wall 6az), so all three keep
 * scalar extern "C" declarations rather than becoming callable methods. */
#include "daPgDfdr_c.h"
#include "decl_common.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"

struct BMD_File;
struct BTP_File;

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *o, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *o, void *f, int i, int fx, unsigned j);
extern void func_ov027_02111994(void *c);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *o, void *kcl, void *m, int fx, short s, void *clps);
extern void func_020393d4(void *p, int v);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *o, void *act, int a, int b, unsigned c, unsigned d);

extern char data_ov027_02113c7c;
extern char data_ov027_02113c94;
extern char data_ov027_02113c6c;
extern void _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
}

s32 daPgDfdr_c::InitResources()
{
    int i;
    void *f;
    Vector3 pos;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov027_02113c7c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, f, 1, -1);

    for (i = 0; i < 3; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov027_02112ca4[i]);

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov027_02113c94);
    TextureSequence::Prepare(**(BMD_File**)(&data_ov027_02113c7c + 4),
                             **(BTP_File**)(&data_ov027_02113c94 + 4));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, *(void**)(&data_ov027_02113c94 + 4), 0, 0x1000, 0);

    mAngleY = (short)0xdd30;
    mPrevAngleY = mAngleY;
    mPosX = 0x6c4000;
    mPosY = 0xcb2000;
    mPosZ = 0x182bb8;
    func_ov027_02111994(this);

    f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov027_02113c6c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, f, &mClsnMat, 0x199, mAngleY, &data_ov027_021130e8);
    func_020393d4(&mMeshCollider, (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x82000, 0xc8000, 0x800004, 0);
    func_ov027_02111d70((char*)this, 1);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return 1;
}
