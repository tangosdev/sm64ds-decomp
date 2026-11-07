//cpp
/**
 * d_a_obj_c1_hikari.cpp
 * Object - Castle light beam
 *
 * Castle light beam (profile C1_HIKARI / LIGHT_BEAM(37)). ov010 also
 * C1_TRAP / C1_PEACH / ROTATING_COG_BIG. RTTI ov010:0x02112b64 names
 * daObjC1Hikari_c as a direct dActor_c subclass; ugly RTTI name is final.
 *
 * The eight functions are defined in reverse ROM order because mwccarm
 * emits ordinary function sections in reverse source order. InitResources is
 * the class's out-of-line key function; its vtable references the inline
 * destructor in daObjC1Hikari_c.h, which makes the compiler emit retail's D1
 * then D0 pair without a D2 or a forcing helper.
 *
 * What stays, and why:
 * - VecFx32 POD locals; Vector3's empty destructor would add
 *   _ZN7Vector3D1Ev to this TU
 * - gC1HikariModelFile is the model SharedFilePtr this Init LoadFile's,
 *   defined at the end of this file (constructed as file ID 1453)
 * - data_0209caa0.file.flags1 & 0x80000000; this Init is the consumer, no recovered
 *   name for that save-flag bit
 * - Sound::PlaySmallSecretSound stays TU-local; Sound.h has no declaration.
 *   MulVec3Mat4x3, Vec3_Add, AngleDiff and NumStars have no shared header
 *   either, so they keep a local extern "C" block.
 * - S14 no g_profile_C1_HIKARI (sinit / data outside this .text)
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_c1_hikari_AssetId { ASSET_C1_HIKARI_BMD = 0x05ad };

#include "daObjC1Hikari_c.h"
#include "Message.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx.h"
#include "SaveBlock.h"

/* Behavior's copies of the two world-space offsets it rotates through
 * data_020a0e68 are VecFx32. The shared Vector3 has an empty destructor,
 * which would add an unrelated compiler helper to this TU. */
/* The beam's file-scope SharedFilePtr object, spelled through the same
   wrapper idiom the landed folds use: the handle's code is the veneer pair
   the cartridge already carries, so the wrapper gets no bodies of its own.
   The constructor aliases func_02017acc and the destructor func_02017ab4 in
   this unit's manifest. */
struct C1HikariModelFilePtr : SharedFilePtr {
    C1HikariModelFilePtr(unsigned int id);
    ~C1HikariModelFilePtr();
};

/* Defined at the end of this file so its constructor does not enter .text. */
extern C1HikariModelFilePtr gC1HikariModelFile;
extern SaveBlock data_0209caa0;

/* local extern: arm9 math and save helpers with no shared header. */
extern "C" {
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *res);
extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern int AngleDiff(int a, int b);
extern unsigned char NumStars(void);
}

namespace Sound {
int PlaySmallSecretSound(dActor_c *actor, u16 *counter);
}

/* Reconstructed source-style name: SM64DS proves daObjC1Hikari_c through RTTI,
 * allocation size, vtable identity, and the C1_HIKARI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: LightBeam_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * fBase_c::operator new(unsigned long) keeps the allocation on
 * _ZN7fBase_cnwEm; without it the call relocates to the unavailable global
 * `_Znwm`. */
extern "C" daObjC1Hikari_c *daObjC1Hikari_c_classInit() {
    return new daObjC1Hikari_c();
}

int daObjC1Hikari_c::InitResources() {
    dExtSimpleModel_c::LoadFile(gC1HikariModelFile);
    if (NumStars() < 0xe)
        return 0;
    if (data_0209caa0.file.flags1 & 0x80000000)
        return 0;
    mModel.SetFile((BMD_File *)gC1HikariModelFile.filePtr, 1, 0x13);
    mCylinder.Init(this, mPos, 0xf0000, 0x8c000, 0x4800002, 0);
    UpdateModelTransform();
    return 1;
}

int daObjC1Hikari_c::Behavior() {
    VecFx32 vIn, vMid, vRes;
    VecFx32 hv;
    VecFx32 vIn2, vMid2, vRes2;

    if (mTalkPlayer != 0) {
        daPly_c *p;
        if (!Sound::PlaySmallSecretSound(this, mSoundTimers))
            return 1;
        p = mTalkPlayer;

        vIn.x = -0x3b0000;
        vIn.y = 0x200000;
        vIn.z = 0x120000;
        vMid.x = 0;
        vMid.y = 0;
        vMid.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3((Vector3 *)&vIn, &data_020a0e68, (Vector3 *)&vMid);
        Vec3_Add((Vector3 *)&vRes, (const Vector3 *)&mPosX, (const Vector3 *)&vMid);

        switch (p->GetTalkState()) {
        case 0:
            if (ApproachLinear(p->mAngleY, mAngleY + 0x8000, 0x800)) {
                Message::PrepareTalk();
                p->ShowMessage(*this, 0x192, (const Vector3 *)&vRes, 0, 0);
            }
            break;
        case 1:
            break;
        default:
            Message::EndTalk();
            mTalkPlayer = 0;
            break;
        }
    } else {
        if (mCylinder.hitFlags & 0x8000000) {
            dActor_c *o = dActor_c::FindWithID(mCylinder.otherOwner);
            if (o) {
                int b = (int)(o->actorID == 0xbf);
                if (b) {
                    VecFx32 *op = (VecFx32 *)&o->mPosX;
                    hv = *op;
                    if (AngleDiff(Vec3_HorzAngle(&mPos, (const Vector3 *)&hv), mAngleY) < 0x4000) {
                        daPly_c *player = static_cast<daPly_c *>(o);
                        if (player->mSurfaceType == 0xd) {
                            if (player->StartTalk(*this, false)) {
                                mTalkPlayer = player;
                                mSoundTimers[0] = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    vIn2.x = -0x3b0000;
    vIn2.y = 0;
    vIn2.z = 0x320000;
    vMid2.x = 0;
    vMid2.y = 0;
    vMid2.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3((Vector3 *)&vIn2, &data_020a0e68, (Vector3 *)&vMid2);
    Vec3_Add((Vector3 *)&vRes2, (const Vector3 *)&mPosX, (const Vector3 *)&vMid2);
    mCylinder.pos.x = vRes2.x;
    mCylinder.pos.y = vRes2.y;
    mCylinder.pos.z = vRes2.z;
    mCylinder.radius = 0xf0000;
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}

int daObjC1Hikari_c::Render() {
    mModel.Render(0);
    return 1;
}

int daObjC1Hikari_c::CleanupResources() {
    gC1HikariModelFile.Release();
    return 1;
}

void daObjC1Hikari_c::UpdateModelTransform() {
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

/* The initializer constructs the model handle with file ID 1453 and
 * registers its destructor; the registration node is a compiler
 * temporary. */
C1HikariModelFilePtr gC1HikariModelFile(ASSET_C1_HIKARI_BMD);
