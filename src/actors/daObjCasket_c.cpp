//cpp
/* Translation unit for ov071/daObjCasket_c, hand-curated.
 * 14 function(s), .text 0x02121fe4..0x021226a0.
 *
 * The casket in Big Boo's Haunt (profile CASKET): a dBgActor_c lying on its
 * back that stands up on end when a player comes near, shakes for a moment,
 * and falls back down with a thud and a camera shake. Behavior runs one of
 * two states out of a table of { init, exec, name } entries at
 * data_ov071_02122ecc, which __sinit_ov071_02122a64 fills:
 *
 *   0 "WAIT"     init func_ov071_021223b0  exec func_ov071_021221bc
 *   1 "STANDUP"  init func_ov071_02122194  exec func_ov071_021220c8
 *
 * NAME: daObjCasket_c is the cartridge's RTTI spelling. _ZTS at ov071
 * 0x02122ea0 is the string "13daObjCasket_c", and the _ZTI at 0x02122e94
 * names the vtable at 0x02122efc as this class's. The tree called the class
 * Coffin until then.
 *
 * THE DESTRUCTOR IS INLINE AND EMPTY in the class header, so InitResources is
 * the key function and this TU emits _ZTV13daObjCasket_c, _ZTI13daObjCasket_c
 * and _ZTS13daObjCasket_c with the inherited bases' RTTI records. D1 and D0 are
 * emitted from the header, so there is no destructor text here to mark.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Leftover: dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange and
 *   dActor_c::Earthquake stay mangled. Each takes Fix12<int> by value (wall
 *   6az), and a member call homes the argument and changes the code.
 * Leftover: the four state bodies and the two table helpers are called as
 *   members of this class, but they keep their func_ov071_* linker names as
 *   C-linkage helpers over a daObjCasket_c pointer, as does the model-matrix
 *   update func_ov071_02122080. A member spelling would coin seven mangled
 *   names; that renaming is separate work.
 * Leftover: the state table (data_ov071_02122ecc) and the model and
 *   collision files (data_ov071_021230d0, data_ov071_021230d8) are unnamed
 *   ov071 rows this TU does not own.
 */

#include "daObjCasket_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "Player.h"

typedef void (daObjCasket_c::*StateFunc)();

struct CasketState {
    StateFunc init;
    StateFunc exec;
    const char *name;
};

int ApproachLinear(short &value, short target, short step);

extern "C" {
/* The state table, indexed by mState. */
extern CasketState data_ov071_02122ecc[];
/* The casket's model and collision files. */
extern SharedFilePtr data_ov071_021230d0;
extern SharedFilePtr data_ov071_021230d8;
/* The CLPS block handed to dBgW_KcMbg::SetFile. */
extern CLPS_Block data_ov063_0211ebd8;
/* Scratch rotation matrix. */
extern Matrix4x3 data_020a0e68;

void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *out);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_020393d4(int *p, int v);

void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 &mat, int scale,
    short angleY, CLPS_Block &clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, Vector3 *pos, int strength);

void func_ov071_02122080(daObjCasket_c *casket);
void func_ov071_021223c8(daObjCasket_c *casket, int state);
void func_ov071_02122414(daObjCasket_c *casket);
}

/* -------------------------------------------------------------------------- */
// @symbol daObjCasket_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjCasket_c through RTTI,
 * allocation size, vtable identity, and the CASKET registry profile; later EAD
 * lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: Coffin_Spawn. */
extern "C" daObjCasket_c *daObjCasket_c_classInit()
{
    return new daObjCasket_c();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c13InitResourcesEv
int daObjCasket_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov071_021230d0), 1, -1);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;

    /* The model's origin is at the foot; the actor sits 200 units along its
       facing from there. */
    Vector3 in;
    Vector3 out;
    in.x = 0;
    in.y = 0;
    in.z = 0xc8000;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    Vector3 res;
    Vec3_Add(&res, (Vector3 *)&mPosX, &out);
    mPosX = res.x;
    mPosY = res.y;
    mPosZ = res.z;

    func_ov071_02122080(this);
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(data_ov071_021230d8),
        mClsnMat, 0x199, mAngleY, data_ov063_0211ebd8);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c8BehaviorEv
int daObjCasket_c::Behavior()
{
    /* Setting 1 is a casket that never moves. */
    if ((param1 & 0xff) == 1) {
        func_ov071_02122080(this);
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
            UpdateClsnPosAndRot();
        return 1;
    }

    mBehaviorTimer++;
    func_ov071_02122414(this);
    func_ov071_02122080(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c6RenderEv
int daObjCasket_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c16CleanupResourcesEv
int daObjCasket_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov071_021230d0.Release();
    data_ov071_021230d8.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov071_02122414
/* Runs the current state's exec function. */
extern "C" void func_ov071_02122414(daObjCasket_c *casket)
{
    (casket->*data_ov071_02122ecc[casket->mState].exec)();
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov071_021223c8
/* Enters a state and runs its init function. */
extern "C" void func_ov071_021223c8(daObjCasket_c *casket, int state)
{
    casket->mState = state;
    (casket->*data_ov071_02122ecc[casket->mState].init)();
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov071_021223b0
/* State 0 init, WAIT. */
extern "C" void func_ov071_021223b0(daObjCasket_c *casket)
{
    casket->mAngleStep = 0;
    casket->mStateTimer = 60;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov071_021221bc
/* State 0 exec, WAIT. While still tilted, swing back down, gathering speed,
 * and land with a camera shake and a puff of dust at the head. Once flat,
 * stand up again when a player comes within 300 units of the head and the
 * wait timer has run out. */
extern "C" void func_ov071_021221bc(daObjCasket_c *casket)
{
    Vector3 vin, vout;
    Vector3 pp;
    Vector3 din, dout;
    Vector3 eq;
    Vector3 ld;

    if (casket->mAngleX != 0) {
        ApproachLinear(casket->mAngleStep, -0x7d0, 0xc8);
        if (ApproachLinear(casket->mAngleX, 0, -casket->mAngleStep) == 0)
            return;
        eq.x = casket->mPosX;
        eq.y = casket->mPosY;
        eq.z = casket->mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(casket, &eq, 0x5dc000);

        vin.x = 0; vin.y = 0; vin.z = -0xc8000;
        vout.x = 0; vout.y = 0; vout.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, casket->mAngleY);
        MulVec3Mat4x3(&vin, &data_020a0e68, &vout);
        AddVec3(&vout, (Vector3 *)&casket->mPosX, &vout);
        ld.x = vout.x; ld.y = vout.y; ld.z = vout.z;
        casket->LandingDustAt(ld, true);
        Sound::PlayBank3(0x5a, *(Vector3 *)&casket->mCamSpacePosX);
    } else {
        Vector3 *plp;
        Player *pl = casket->ClosestPlayer();
        if (pl == 0)
            return;
        /* Copied through a pointer: read straight off the player, the copy
           comes out one instruction short. */
        plp = (Vector3 *)&pl->mPosX;
        pp.x = plp->x;
        pp.y = plp->y;
        pp.z = plp->z;
        din.x = 0; din.y = 0x64000; din.z = -0xc8000;
        dout.x = 0; dout.y = 0; dout.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, casket->mAngleY);
        MulVec3Mat4x3(&din, &data_020a0e68, &dout);
        AddVec3(&dout, (Vector3 *)&casket->mPosX, &dout);
        if (Vec3_HorzDist(&dout, &pp) >= 0x12c000)
            return;
        if (DecIfAbove0_Short(&casket->mStateTimer) != 0)
            return;
        func_ov071_021223c8(casket, 1);
    }
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov071_02122194
/* State 1 init, STANDUP. */
extern "C" void func_ov071_02122194(daObjCasket_c *casket)
{
    casket->mAngleStep = 0;
    casket->mStateTimer = 60;
    Sound::PlayBank3(0x58, *(Vector3 *)&casket->mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov071_021220c8
/* State 1 exec, STANDUP. Swing up to stand on end, gathering speed; then
 * shake from side to side, rattling every fourth frame, and go back to WAIT
 * when the timer runs out. */
extern "C" void func_ov071_021220c8(daObjCasket_c *casket)
{
    if (casket->mAngleX != 0x4000) {
        ApproachLinear(casket->mAngleStep, 0x3e8, 0xc8);
        ApproachLinear(casket->mAngleX, 0x4000, casket->mAngleStep);
        return;
    }
    DecIfAbove0_Short(&casket->mStateTimer);
    if (casket->mStateTimer == 0) {
        func_ov071_021223c8(casket, 0);
        casket->mAngleZ = 0;
        return;
    }
    if (casket->mStateTimer > 30)
        return;
    if (casket->mBehaviorTimer % 4 == 0)
        Sound::PlayBank3(0x59, *(Vector3 *)&casket->mCamSpacePosX);
    casket->mAngleZ = (casket->mBehaviorTimer & 1) * 0x190 - 0xc8;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov071_02122080
/* Puts the model where the actor is. */
extern "C" void func_ov071_02122080(daObjCasket_c *casket)
{
    Matrix4x3_FromRotationXYZExt(&casket->mModel.mat4x3,
                                 casket->mAngleX, casket->mAngleY, casket->mAngleZ);
    casket->mModel.mat4x3.m[9] = casket->mPosX >> 3;
    casket->mModel.mat4x3.m[10] = casket->mPosY >> 3;
    casket->mModel.mat4x3.m[11] = casket->mPosZ >> 3;
}
