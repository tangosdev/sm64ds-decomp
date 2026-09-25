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
 *   0 "WAIT"     init St_Wait_Init     exec St_Wait_Main
 *   1 "STANDUP"  init St_StandUp_Init  exec St_StandUp_Main
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
 *   dActor_c::Earthquake stay mangled bridges. Each takes Fix12<int> by
 *   value, and a member call puts it on the stack: SetFile as a member grows
 *   InitResources from 0x114 to 0x120 bytes, as in
 *   notes/experiments/batch2-2685-dossunbar-setfile.md.
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

    UpdateModelTransform();
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
        UpdateModelTransform();
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
            UpdateClsnPosAndRot();
        return 1;
    }

    mBehaviorTimer++;
    RunState();
    UpdateModelTransform();
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
// @symbol _ZN13daObjCasket_c8RunStateEv
/* Runs the current state's exec function. */
void daObjCasket_c::RunState()
{
    (this->*data_ov071_02122ecc[mState].exec)();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c8SetStateEi
/* Enters a state and runs its init function. */
void daObjCasket_c::SetState(int state)
{
    mState = state;
    (this->*data_ov071_02122ecc[mState].init)();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c12St_Wait_InitEv
/* State 0 init, WAIT. */
void daObjCasket_c::St_Wait_Init()
{
    mAngleStep = 0;
    mStateTimer = 60;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c12St_Wait_MainEv
/* State 0 exec, WAIT. While still tilted, swing back down, gathering speed,
 * and land with a camera shake and a puff of dust at the head. Once flat,
 * stand up again when a player comes within 300 units of the head and the
 * wait timer has run out. */
void daObjCasket_c::St_Wait_Main()
{
    Vector3 vin, vout;
    Vector3 pp;
    Vector3 din, dout;
    Vector3 eq;
    Vector3 ld;

    if (mAngleX != 0) {
        ApproachLinear(mAngleStep, -0x7d0, 0xc8);
        if (ApproachLinear(mAngleX, 0, -mAngleStep) == 0)
            return;
        eq.x = mPosX;
        eq.y = mPosY;
        eq.z = mPosZ;
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &eq, 0x5dc000);

        vin.x = 0; vin.y = 0; vin.z = -0xc8000;
        vout.x = 0; vout.y = 0; vout.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3(&vin, &data_020a0e68, &vout);
        AddVec3(&vout, (Vector3 *)&mPosX, &vout);
        ld.x = vout.x; ld.y = vout.y; ld.z = vout.z;
        LandingDustAt(ld, true);
        Sound::PlayBank3(0x5a, *(Vector3 *)&mCamSpacePosX);
    } else {
        Vector3 *plp;
        Player *pl = ClosestPlayer();
        if (pl == 0)
            return;
        /* Through a pointer: copied straight off the player, it is one instruction short. */
        plp = (Vector3 *)&pl->mPosX;
        pp.x = plp->x;
        pp.y = plp->y;
        pp.z = plp->z;
        din.x = 0; din.y = 0x64000; din.z = -0xc8000;
        dout.x = 0; dout.y = 0; dout.z = 0;
        Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
        MulVec3Mat4x3(&din, &data_020a0e68, &dout);
        AddVec3(&dout, (Vector3 *)&mPosX, &dout);
        if (Vec3_HorzDist(&dout, &pp) >= 0x12c000)
            return;
        if (DecIfAbove0_Short(&mStateTimer) != 0)
            return;
        SetState(1);
    }
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c15St_StandUp_InitEv
/* State 1 init, STANDUP. */
void daObjCasket_c::St_StandUp_Init()
{
    mAngleStep = 0;
    mStateTimer = 60;
    Sound::PlayBank3(0x58, *(Vector3 *)&mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c15St_StandUp_MainEv
/* State 1 exec, STANDUP. Swing up to stand on end, gathering speed; then
 * shake from side to side, rattling every fourth frame, and go back to WAIT
 * when the timer runs out. */
void daObjCasket_c::St_StandUp_Main()
{
    if (mAngleX != 0x4000) {
        ApproachLinear(mAngleStep, 0x3e8, 0xc8);
        ApproachLinear(mAngleX, 0x4000, mAngleStep);
        return;
    }
    DecIfAbove0_Short(&mStateTimer);
    if (mStateTimer == 0) {
        SetState(0);
        mAngleZ = 0;
        return;
    }
    if (mStateTimer > 30)
        return;
    if (mBehaviorTimer % 4 == 0)
        Sound::PlayBank3(0x59, *(Vector3 *)&mCamSpacePosX);
    mAngleZ = (mBehaviorTimer & 1) * 0x190 - 0xc8;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjCasket_c20UpdateModelTransformEv
/* Puts the model where the actor is. */
void daObjCasket_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3,
                                 mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}
