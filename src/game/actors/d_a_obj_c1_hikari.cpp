//cpp
/**
 * d_a_obj_c1_hikari.cpp
 * Object - Castle light beam
 *
 * deslop
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
 * Leftover:
 * - dCcAcPos_c::Init stays mangled (Fix12<int> by value, wall 6az; this
 *   InitResources passes 0xf0000 / 0x8c000 / 0x4800002)
 * - HikariVec3 POD locals; Vector3's empty destructor would add
 *   _ZN7Vector3D1Ev to this TU
 * - (Vector3 *)&mPosX; dActor_c has no Pos() on this branch (S18)
 * - data_ov010_02112d50 is the model SharedFilePtr this Init LoadFile's;
 *   ov010 sinit constructs it as file ID 1453. SharedFilePtr.h has no fields
 *   and decl_common's plurality is int[], so the LoadFile/Release/SetFile
 *   puns stay here
 * - data_0209caa0[1] & 0x80000000; this Init is the consumer, no recovered
 *   name for that save-flag bit
 * - Sound::PlaySmallSecretSound stays TU-local; Sound.h has no declaration
 * - S14 no g_profile_C1_HIKARI (sinit / data outside this .text)
 */

#include "daObjC1Hikari_c.h"
#include "Message.h"
#include "Player.h"
#include "SharedFilePtr.h"

/* POD copies of the two world-space offsets this Behavior rotates through
 * data_020a0e68. The shared Vector3 has an empty destructor, which would add
 * an unrelated compiler helper to this TU. */
struct HikariVec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Vec3;

extern "C" {
extern void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *res);
extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern int AngleDiff(int a, int b);
extern unsigned char NumStars(void);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset,
    int radius, int height, unsigned int flags, unsigned int vulnFlags);

extern Matrix4x3 data_020a0e68;
extern int data_ov010_02112d50[];
extern int data_0209caa0[];
}

namespace Sound {
int PlaySmallSecretSound(dActor_c *actor, u16 *counter);
}

int ApproachLinear(s16 &val, s16 target, s16 step);

extern int _ZTV15daObjC1Hikari_c[];

/* Reconstructed source-style name: SM64DS proves daObjC1Hikari_c through RTTI,
 * allocation size, vtable identity, and the C1_HIKARI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: LightBeam_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * fBase_c::operator new(unsigned long) keeps the allocation on
 * _ZN7fBase_cnwEj; without it the call relocates to the unavailable global
 * `_Znwm`. */
// @symbol daObjC1Hikari_c_classInit
extern "C" daObjC1Hikari_c *daObjC1Hikari_c_classInit()
{
    return new daObjC1Hikari_c();
}

// @symbol _ZN15daObjC1Hikari_c13InitResourcesEv
int daObjC1Hikari_c::InitResources()
{
    Model::LoadFile(*(SharedFilePtr *)data_ov010_02112d50);
    if (NumStars() < 0xe)
        return 0;
    if (data_0209caa0[1] & 0x80000000)
        return 0;
    mModel.SetFile((BMD_File *)data_ov010_02112d50[1], 1, 0x13);
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mCylinder, this, (const Vector3 *)&mPosX, 0xf0000, 0x8c000, 0x4800002, 0);
    UpdateModelTransform();
    return 1;
}

// @symbol _ZN15daObjC1Hikari_c8BehaviorEv
int daObjC1Hikari_c::Behavior()
{
    HikariVec3 vIn, vMid, vRes;
    HikariVec3 hv;
    HikariVec3 vIn2, vMid2, vRes2;

    if (mTalkPlayer != 0) {
        Player *p;
        if (!Sound::PlaySmallSecretSound(this, mSoundTimers))
            return 1;
        p = (Player *)mTalkPlayer;

        vIn.x = -0x3b0000;
        vIn.y = 0x200000;
        vIn.z = 0x120000;
        vMid.x = 0;
        vMid.y = 0;
        vMid.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3((Vector3 *)&vIn, &data_020a0e68, (Vector3 *)&vMid);
        Vec3_Add((Vec3 *)&vRes, (Vec3 *)&mPosX, (Vec3 *)&vMid);

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
                    HikariVec3 *op = (HikariVec3 *)&o->mPosX;
                    hv = *op;
                    if (AngleDiff(Vec3_HorzAngle((const Vector3 *)&mPosX, (const Vector3 *)&hv), mAngleY) < 0x4000) {
                        if (((Player *)o)->mSurfaceType == 0xd) {
                            if (((Player *)o)->StartTalk(*this, false)) {
                                mTalkPlayer = o;
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
    Vec3_Add((Vec3 *)&vRes2, (Vec3 *)&mPosX, (Vec3 *)&vMid2);
    mCylinder.pos.x = vRes2.x;
    mCylinder.pos.y = vRes2.y;
    mCylinder.pos.z = vRes2.z;
    mCylinder.radius = 0xf0000;
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}

// @symbol _ZN15daObjC1Hikari_c6RenderEv
int daObjC1Hikari_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObjC1Hikari_c16CleanupResourcesEv
int daObjC1Hikari_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov010_02112d50)->Release();
    return 1;
}

// @symbol _ZN15daObjC1Hikari_c20UpdateModelTransformEv
void daObjC1Hikari_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}
