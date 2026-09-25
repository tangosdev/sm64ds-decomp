//cpp
/* daObjKey_c, ov089. Matched run 0x02131b18..0x021327d0, ten functions:
 * the state body StateDrop, the pickup helpers func_ov089_02131dcc and
 * func_ov089_02131df4, UpdateModelTransform and the virtual overrides.
 *
 * NAME: _ZTS10daObjKey_c is the cartridge string "10daObjKey_c" at
 * 0x02132b58. _ZTI10daObjKey_c at 0x02132b4c reads [__si_class_type_info,
 * 0x02132b58, _ZTI12dEnemyBase_c], so the single base is dEnemyBase_c, and
 * the word before _ZTV10daObjKey_c (0x02132ba8) is that _ZTI. The class was
 * coined Key before the ROM name was read (factory alias Key_Spawn).
 *
 * PARTIAL: this is not the whole class. The destructors stay shards. D1 at
 * 0x02130f00 and D0 at 0x02130f50 sit in front of func_ov089_0213162c,
 * which byte-matches but is enrolled without a `complete` marker, so the
 * build keeps the cartridge's bytes there and this TU cannot span it.
 * func_ov089_02130fb4, UnloadKeyModels, LoadKeyModels, func_ov089_0213115c
 * and func_ov089_021311c0 stay shards for the same reason. The D1 shard
 * holds the out-of-line destructor, the key function, so it emits
 * _ZTV/_ZTI/_ZTS; this TU defines no destructor and emits no vtable.
 *
 * Source order is ROM-ascending. `#pragma defer_codegen off` emits each
 * function where it stands.
 */

#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "daObjKey_c.h"
#include "Player.h"
#include "Camera.h"
#include "Sound.h"
#include "SharedFilePtr.h"

#pragma defer_codegen off

namespace Event { void SetBit(unsigned int bit); int ClearBit(unsigned int bit); }

/* The state table, filled by __sinit_ov089_021328d4 and indexed by mState. */
typedef void (daObjKey_c::*StateFunc)();
extern StateFunc data_ov089_02132cec[];

/* Mangled bridges below, each kept for its own reason:
 * - Camera::SetFlag_3, Particle::System::New: include/Camera.h and
 *   include/Particle__System.h do not declare them.
 * - dActor_c::DropShadowRadHeight, dCcAcPos_c::Init, ModelAnim::SetAnim:
 *   the real calls pass Fix12<int> by value and do not match.
 * - dBgCh_Actr::Init: include/dBgCh_Actr.h declares it with Fix12i, which
 *   mangles to a different name.
 * - Player::SetNoControlState: include/Player.h declares it and the real
 *   call matches; the local declaration holds the declaration-agreement
 *   plurality that src/game/actors/d_a_wanwan.cpp needs.
 * - Sound::LoadAndSetMusic_Layer3 is declared in include/decl_common.h. */
extern "C" {
extern void dBgCh_Actr_UpdateContinuous_Veneer(char *p);
extern void *data_0209f318;
extern int data_0209b454;
extern void _ZN6Camera9SetFlag_3Ev(Camera *cam);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int c, int d, int e, void *f, void *g);
extern void func_02012694(unsigned int id, const Vector3 *pos);
extern int _ZN6Player17SetNoControlStateEhih(void *p, unsigned char a, int b, unsigned char d);
extern void func_ov002_020c3dbc(void *player);
extern int data_0209caa0[];
void Matrix4x3_FromRotationY(void *m, short angle);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *a, void *sm, void *mtx, int rad, int h, unsigned int x);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern char data_ov089_021328b4[];
extern int data_ov002_02110964;
extern int data_ov089_02132c60[];
extern int data_ov089_02132c40[];
extern int data_ov089_02132c70[];
extern int data_ov089_02132c48[];
extern int data_ov089_02132b40[];
extern int data_ov089_02132ca4[];
extern Matrix4x3 data_020a0e68;
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, int ang);
extern void MulMat4x3Mat4x3(void *d, void *a, void *b);
extern void SubVec3(void *d, void *a, void *b);
extern void Vec3_LslInPlace(void *v, int sh);
extern void AddVec3(void *d, void *a, void *b);
extern void LoadKeyModels(int idx);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *thiz, void *actor, void *pos, int r, int s, unsigned int a, unsigned int b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int a, int b, void *v, void *w);
extern char data_ov002_0211094c;
extern int data_0209cef0;
void func_ov089_02131df4(char *c, char *p);
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c9StateDropEv, 0x02131b18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c9StateDropEv
void daObjKey_c::StateDrop()
{
    Camera *cam = (Camera *)data_0209f318;
    Vector3 v;

    dBgCh_Actr_UpdateContinuous_Veneer((char *)&mWithMeshClsn);

    switch (mStep) {
    case 0:
        {
            /* Pointer copies: indexing cam directly folds the offsets into the loads. */
            Vector3 *lookAt = &cam->lookAt;
            Vector3 *pos = &cam->pos;
            mFlags |= 0x4000000;
            data_0209b454 |= 0x4000000;
            mSavedCamLookAt.x = lookAt->x;
            mSavedCamLookAt.y = lookAt->y;
            mSavedCamLookAt.z = lookAt->z;
            mSavedCamPos.x = pos->x;
            mSavedCamPos.y = pos->y;
            mSavedCamPos.z = pos->z;
            _ZN6Camera9SetFlag_3Ev(cam);
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            v.y = v.y + 0x64000;
            cam->SetLookAt(v);
            mStep++;
            return;
        }

    case 1:
        /* Offset after the copy: mPosY + 0x64000 in the copy reorders the loads. */
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y = v.y + 0x64000;
        cam->SetLookAt(v);
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        v.y = v.y + 0x1c000;
        mParticleID[0] = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            mParticleID[0], 0x81, v.x, v.y, v.z, 0, 0);
        if (mWithMeshClsn.JustHitGround()) {
            mVertSpeed = -mVertSpeed >> 1;
            func_02012694(0x36, (Vector3 *)&mCamSpacePosX);
            return;
        }
        if (mWithMeshClsn.IsOnGround() == 0)
            return;
        cam->SetLookAt(mSavedCamLookAt);
        cam->SetPos(mSavedCamPos);
        cam->mFlags &= ~8;
        mFlags &= ~0x4000000;
        data_0209b454 &= ~0x4000000;
        mStep++;
        mdCcAcPos_c.flags &= ~1;
        mParticleID[0] = 0;
        return;

    case 2:
        {
            u32 id = mdCcAcPos_c.otherOwner;
            dActor_c *found;
            if (id == 0)
                return;
            found = dActor_c::FindWithID(id);
            if (found == 0)
                return;
            if ((mdCcAcPos_c.hitFlags & 0x400000) == 0)
                return;
            func_ov089_02131df4((char *)this, (char *)found);
            mStep++;
            return;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* func_ov089_02131dcc, 0x02131dcc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov089_02131dcc
/* The (char *, char *) spelling is the one the other callers declare. */
extern "C" void func_ov089_02131dcc(char *c, char *p)
{
    daObjKey_c *key = (daObjKey_c *)c;

    func_ov002_020c3dbc(p);
    Event::SetBit(0x1d);
    key->MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* func_ov089_02131df4, 0x02131df4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov089_02131df4
extern "C" void func_ov089_02131df4(char *c, char *p)
{
    daObjKey_c *key = (daObjKey_c *)c;
    Player *player = (Player *)p;

    if (data_0209caa0[1] & (2 << key->mState))
        data_0209f2ac = 0;
    else
        data_0209f2ac = 1;
    data_0209caa0[1] |= (2 << key->mState);

    if (key->mState <= 1) {
        _ZN6Player17SetNoControlStateEhih(player, 3, -1, 0);
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x17);
    } else if (key->mState != 7) {
        _ZN6Player17SetNoControlStateEhih(player, 3, -1, 0);
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x17);
    }

    func_ov089_0213115c((char *)key, 3);
    key->mPlayer = player;
    {
        /* Through a pointer: key->mPlayer->mPosX folds the offset into each load. */
        s32 *pos = &key->mPlayer->mPosX;
        key->mPosX = pos[0];
        key->mPosY = pos[1];
        key->mPosZ = pos[2];
        key->mAngleY = key->mPlayer->mAngleY;
        key->mFlags &= ~0x40000;
        Event::SetBit(0x1d);
    }
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c13OnTurnIntoEggER6Player, 0x02131f04 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c13OnTurnIntoEggER6Player
/* Vtable slot 19: ov089 relocs.txt has _ZTV10daObjKey_c (0x02132ba8) + 0x4c
 * -> 0x02131f04. */
void daObjKey_c::OnTurnIntoEgg(Player &player)
{
    /* The flag keeps the ROM's moveq/movne pair. */
    unsigned isKey = (actorID == 0x11a);
    if (isKey)
        return func_ov089_02131df4((char *)this, (char *)&player);
    return func_ov089_02131dcc((char *)this, (char *)&player);
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c13OnYoshiTryEatEv, 0x02131f4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c13OnYoshiTryEatEv
s32 daObjKey_c::OnYoshiTryEat() {
    return 4;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c20UpdateModelTransformEv, 0x02131f54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c20UpdateModelTransformEv
void daObjKey_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = mPosY >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;
    if (((int *)data_ov089_021328b4)[mState] != 0 && mAnimID == 0) {
        Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
        mModel.mat4x3.m[9] = mPosX >> 3;
        mModel.mat4x3.m[10] = (mPosY + 0x64000) >> 3;
        mModel.mat4x3.m[11] = mPosZ >> 3;
    }
    mShadowMatrix = IDENTITY_MATRIX4X3;
    mShadowMatrix.m[9] = mPosX >> 3;
    mShadowMatrix.m[10] = mPosY >> 3;
    mShadowMatrix.m[11] = mPosZ >> 3;
    /* Mangled call: Fix12<int> arguments by value load from the literal pool. */
    if (mModelAnim.file == (BCA_File *)(&data_ov002_02110964)[1])
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(this, &mShadowModel, &mShadowMatrix, 0x96000, 0x3e8000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c16CleanupResourcesEv, 0x02132084 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c16CleanupResourcesEv
int daObjKey_c::CleanupResources()
{
    UnloadKeyModels(mState);
    ((SharedFilePtr *)&data_ov002_02110964)->Release();
    if (mState != 7) {
        ((SharedFilePtr *)data_ov089_02132c60)->Release();
        ((SharedFilePtr *)data_ov089_02132c40)->Release();
        ((SharedFilePtr *)data_ov089_02132c70)->Release();
        ((SharedFilePtr *)data_ov089_02132c48)->Release();
    }
    Event::ClearBit(0x1d);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c6RenderEv, 0x021320f0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c6RenderEv
int daObjKey_c::Render()
{
    int hidden = (mFlags & 0x40000) != 0;
    if (hidden)
        return 1;
    if (mAnimID != 0) {
        mModelAnim.Render(0);
    } else {
        mModelAnim.Render((Vector3 *)&mScaleX);
        if (((int *)data_ov089_021328b4)[mState] != 0 && mAnimID == 0)
            mModel.Render(0);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c8BehaviorEv, 0x02132194 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c8BehaviorEv
int daObjKey_c::Behavior()
{
    Vector3 vec;
    Vector3 offset7;
    Vector3 offset;
    int animID = mAnimID;

    if (animID != 0) {
        if (animID == 3) {
            Player *player = mPlayer;
            if (player != 0) {
                /* Through a pointer: player->mPosX folds the offset into each load. */
                s32 *pos = &player->mPosX;
                mPosX = pos[0];
                mPosY = pos[1];
                mPosZ = pos[2];
                mAngleY = mPlayer->mAngleY;
            }
            if (mModelAnim.Finished() == 0) {
                Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
                MulMat4x3Mat4x3(mModelAnim.data.transforms, &data_020a0e68, &data_020a0e68);
                vec.x = data_020a0e68.m[9];
                vec.y = data_020a0e68.m[10];
                vec.z = data_020a0e68.m[11];
                SubVec3(&vec, &mPosX, &vec);
                Vec3_LslInPlace(&vec, 3);
                AddVec3(&vec, &mPosX, &vec);
                vec.y = *(int *)((char *)mModelAnim.data.bones + 0xc) * 0x23 + vec.y;
                vec.y = vec.y - 0x48000;
                mParticleID[0] = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    mParticleID[0], 0x82, vec.x, vec.y, vec.z, 0, 0);
                mParticleID[1] = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    mParticleID[1], 0x83, vec.x, vec.y, vec.z, 0, 0);
            }
        }

        mModelAnim.Advance();
        UpdateModelTransform();
        if (mModelAnim.Finished()) {
            /* The flag keeps the ROM's moveq/movne pair. */
            int isKey = (actorID == 0x11a);
            if (isKey != 0) {
                if (mModelAnim.file != (BCA_File *)data_ov089_02132c40[1])
                    MarkForDestruction();
            }
        }
        return 1;
    }

    if (UpdateYoshiEat(mWithMeshClsn)) {
        UpdateModelTransform();
        mdCcAcPos_c.Clear();
        return 1;
    }
    mEatingPlayer = 0;
    if (mSpinSpeed > 0x400)
        mSpinSpeed -= 0x100;
    else if (mSpinSpeed == 0)
        mSpinSpeed = 0x400;
    mAngleY += mSpinSpeed;
    UpdatePos(0);
    (this->*data_ov089_02132cec[mState])();
    UpdateModelTransform();
    mdCcAcPos_c.Clear();
    if (mState == 7) {
        offset7.x = data_ov089_02132b40[0];
        offset7.y = data_ov089_02132b40[1];
        offset7.z = data_ov089_02132b40[2];
        mdCcAcPos_c.SetPosRelativeToActor(offset7);
    } else {
        offset.x = data_ov089_02132ca4[0];
        offset.y = data_ov089_02132ca4[1];
        offset.z = data_ov089_02132ca4[2];
        mdCcAcPos_c.SetPosRelativeToActor(offset);
    }
    mdCcAcPos_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* _ZN10daObjKey_c13InitResourcesEv, 0x021324a4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daObjKey_c13InitResourcesEv
int daObjKey_c::InitResources()
{
    Vector3 offset7;
    Vector3 offset;
    int kind = param1 & 7;
    mState = kind;
    LoadKeyModels(mState);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov002_02110964);

    if (mState == 7) {
        if (mModelAnim.SetFile(*(BMD_File **)(&data_ov002_0211094c + 4), 1, 1) == 0)
            return 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)((char *)&data_ov002_02110964 + 4), 0x40000000, 0x1000, 0);
        mScaleX = 0x2000;
        mScaleY = 0x2000;
        mScaleZ = 0x2000;
        offset7.x = data_ov089_02132b40[0];
        offset7.y = data_ov089_02132b40[1];
        offset7.z = data_ov089_02132b40[2];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &offset7, 0x50000, 0xfa000, 0x800003, 0x8000);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0xa0000, 0, 0, 0);
        mSpinSpeed = 0x400;
        mVertAccel = 0;
        Sound::PlayBank3(0x57, *(Vector3 *)&mCamSpacePosX);
    } else {
        Animation::LoadFile(*(SharedFilePtr *)data_ov089_02132c60);
        Animation::LoadFile(*(SharedFilePtr *)data_ov089_02132c40);
        Animation::LoadFile(*(SharedFilePtr *)data_ov089_02132c70);
        Animation::LoadFile(*(SharedFilePtr *)data_ov089_02132c48);
        if (mModelAnim.SetFile(*(BMD_File **)((char *)data_ov089_02132894[mState] + 4), 1, 1) == 0)
            return 0;
        {
            void *extra = (void *)((int *)data_ov089_021328b4)[mState];
            if (extra != 0) {
                if (mModel.SetFile(*(BMD_File **)((char *)extra + 4), 1, -1) == 0)
                    return 0;
            }
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)((char *)&data_ov002_02110964 + 4), 0x40000000, 0x1000, 0);
        mScaleX = 0x3000;
        mScaleY = 0x3000;
        mScaleZ = 0x3000;
        mVertSpeed = 0x23000;
        offset.x = data_ov089_02132ca4[0];
        offset.y = data_ov089_02132ca4[1];
        offset.z = data_ov089_02132ca4[2];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &offset, 0x50000, 0x64000, 0x800003, 0x8000);
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x48000, 0, 0, 0);
        mVertAccel = -0x2000;
        mSpinSpeed = 0;
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mWithMeshClsn.SetLimMovFlag();
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    mStateTimer = 0;
    mStep = 0;
    mBounceCount = 0;
    mTerminalVelocity = -0x32000;
    mAnimID = 0;
    mPlayer = 0;
    mParticleID[0] = mParticleID[1] = mParticleID[2] = 0;
    if (data_0209cef0 == 0)
        Event::ClearBit(0x1d);
    return 1;
}
