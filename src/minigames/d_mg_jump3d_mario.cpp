//cpp
/* dMgJump3DMario_c: one of the three Marios the two jump minigames
 * (Bounce and Pounce, MG_JUMP and MG_JUMP2) hold as mPlayers[3].
 *
 * A pointer-to-member state machine: mState points at the current State*
 * method. Each update applies gravity, integrates mVel into mPos, runs the
 * current state and re-projects the screen position. Tap near Mario and he
 * bounces; miss and he damps out and respawns.
 *
 * Source order is ROM order, which `#pragma defer_codegen off` keeps; do not
 * reorder. The destructor is not virtual, so the first declared virtual
 * (GetPos) is the key function and this file emits the vtable.
 *
 * decl_common.h is left out because four of the data symbols here collide
 * with its spellings.
 *
 * ov006 0x020c6f8c..0x020c8a30 (.text), 40 functions. The first twelve are
 * the helpers the owning scenes call: a tuning-word rescale
 * (Jump3D_SetTuning), the draw/update/reset/init loops over the
 * three-Mario array and its small state accessors (0x020c70d0..0x020c7574).
 * They share the .data tuning words at 0213b008..0213b01c and the bss
 * window at 02140400.. with the members below, which is what puts them in
 * this TU. They were one-function shards, one per function.
 *
 * leftovers:
 * - The helper functions and data_ov006_ tables have no header or name yet.
 * - The int* zeroing of Mtx in Draw and the `int t` temp in LoadResources
 *   are codegen-pinned; each site carries its measured note.
 */

#pragma defer_codegen off

#include "common.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "MgCamera.h"
#include "dMgJump3DMario_c.h"
#include "SharedFilePtr.h"
#include "OAM.h"
#include "OamAttr.h"
#include "PlayerInput.h"
#include "fBase_c.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Matrix2x2.h"
#include "math/Approach.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

static const int kWallX = 0x6c000;   /* mPos.x clamp: the arena walls */
static const int kScreenYMax = 0xbc; /* past this mScreenY the Mario damps out */
static const int kAnimSpeed = 0x800; /* SetAnim speed every install uses */

/* Everything this file calls or reads that no header declares. Keep it
 * above the first function. */
extern "C" {

void func_02012718(int a, int b);

/* Tuning words, read as plain ints. */
int sJump3DBounceVy = 0x5000;
int sJump3DRiseVy = 0x4c00;
int sJump3DStartVx = 0x180;
int sJump3DFallSpeed = 0xffffdc00;
int sJump3DGravityStep = 224;
int sJump3DMoveSpeed = 0x200;
int sJump3DMariosLeft;
int sJump3DRisenOutCount;

/* The six anim files LoadResources loads. The destructor clears only
 * five of them; 0214041c survives in the ROM too. */
BCA_File *sJump3DAnimSdown;
BCA_File *sJump3DAnimDoubleJump1;
BCA_File *sJump3DAnimRollJump;
BCA_File *sJump3DAnimWallJump;
BCA_File *sJump3DAnimStruggle;
BCA_File *sJump3DAnimJump;

/* Slots the current mAnimIdx (0/1/2) picks the anim file from. */
BCA_File **sJump3DAnimTable[] = {&sJump3DAnimJump, &sJump3DAnimDoubleJump1, &sJump3DAnimWallJump};

/* Six anim handles call SharedFilePtr::Construct. The model handle calls
   func_02017a24. */
struct Jump3DMarioAnimFilePtr : SharedFilePtr {
    Jump3DMarioAnimFilePtr(unsigned fileId);
    ~Jump3DMarioAnimFilePtr();
};
struct Jump3DModelFilePtr : SharedFilePtr {
    Jump3DModelFilePtr(unsigned fileId);
    ~Jump3DModelFilePtr();
};
typedef char Jump3DMarioAnimFilePtr_size[sizeof(Jump3DMarioAnimFilePtr) == 8 ? 1 : -1];
typedef char Jump3DModelFilePtr_size[sizeof(Jump3DModelFilePtr) == 8 ? 1 : -1];

extern Jump3DMarioAnimFilePtr sJump3DMarioWallJumpAnimFile;
extern Jump3DMarioAnimFilePtr sJump3DMarioSdownAnimFile;
extern Jump3DMarioAnimFilePtr sJump3DMarioStruggleAnimFile;
extern Jump3DModelFilePtr gMarioModelMgModelFile;
extern Jump3DMarioAnimFilePtr sJump3DMarioRollJumpAnimFile;
extern Jump3DMarioAnimFilePtr sJump3DMarioJumpAnimFile;
extern Jump3DMarioAnimFilePtr sJump3DMarioDoubleJump1AnimFile;

/* The pool the minigame scenes hand in: count, base pointer, level, round
 * and the three player slots. */
int sJump3DArrangeLevel;
int sJump3DLayout;
int sJump3DMarioCount;
char *sJump3DMarioPool;
extern char *sJumpAdapters[];

/* Input gate, spin phase and the model sprite list Draw hands OAM::Render. */
u8 data_ov006_02140400;
u16 sJump3DSpinAngle;
extern struct MgCamera *sMgMainCamera;
extern MgCamera *sMgCurCamera;
extern "C" OamAttr data_ov006_02134d20[1];
OamAttr *data_ov006_02134d1c = data_ov006_02134d20;
extern Vector3 sJump3DMarioScale;
extern char sJump3DRoundLayouts[];

extern fBase_c *gCurrentScene;

} /* extern "C" */
void Sound_PlayBank1Panned(int a, int b, int c);
int func_020179b4(SharedFilePtr *f, dExtModel_c *m, int extra);
void D3D_PlaySeAtX(int a, int b);
void Puff_Spawn(int a, int b);
void Puff_DrawAll(void);
void Puff_UpdateAll(void);
void Puff_ClearAll(int a, OamAttr **b);

/* Rescales the six tuning words at 0213b008..0213b01c from one input,
 * clamped to 0x3000. */
void Jump3D_SetTuning(int a) {
    int b;
    if (a > 0x3000) a = 0x3000;
    b = FX_Sqrt(a);
    sJump3DFallSpeed = FX_MUL(b, -0x2400);
    sJump3DGravityStep = FX_MUL(a, 0xe0);
    sJump3DRiseVy = FX_MUL(b, 0x4c00);
    sJump3DMoveSpeed = FX_MUL(b, 0x200);
    sJump3DStartVx = FX_MUL(b, 0x180);
    sJump3DBounceVy = FX_MUL(b, 0x5000);
}

void Jump3D_DrawAll(void) {
    int i;
    for (i = 0; i < sJump3DMarioCount; i++) {
        ((dMgJump3DMario_c *)(sJump3DMarioPool + i * 0xb8))->Draw();
    }
    Puff_DrawAll();
}

void Jump3D_UpdateAll(void) {
    int i;
    sJump3DSpinAngle = sJump3DSpinAngle + 0x800;
    for (i = 0; i < sJump3DMarioCount; i++) {
        ((dMgJump3DMario_c *)(sJump3DMarioPool + i * 0xb8))->Update();
    }
    Puff_UpdateAll();
}

extern "C" void Jump3D_Arrange(u32 a0, u32 a1)
{
    int i;
    int x;
    sJump3DArrangeLevel = a0;
    sJump3DLayout = a1;
    if (a0 < 5) {
        x = a0 * 0x140 + 0x1000;
    } else {
        x = (a0 % 5 + a0 / 5) * 0x280 + 0x1000;
    }
    Jump3D_SetTuning(x);
    if ((u32)sJump3DArrangeLevel % 5 == 0) {
        sJump3DMariosLeft = sJump3DMarioCount;
    }
    for (i = 0; i < sJump3DMarioCount; i++) {
        ((dMgJump3DMario_c *)(sJump3DMarioPool + i * 0xb8))->HideAndIdle();
    }
    Puff_ClearAll(sJump3DMarioCount - sJump3DMariosLeft, 0);
    sJump3DRisenOutCount = 0;
}

void Jump3D_DisableInput(void) {
    data_ov006_02140400 = 0;
}

void Jump3D_EnableInput(void) {
    data_ov006_02140400 = 1;
}

void Jump3D_RestoreCount(void) {
    ApproachLinear(sJump3DMariosLeft, sJump3DMarioCount, 1);
}

/* Returns 1 when every Mario in the pool is in StateIdle, else 0. */
int Jump3D_AllIdle(void) {
    int i;
    for (i = 0; i < sJump3DMarioCount; i++) {
        if (((dMgJump3DMario_c *)sJump3DMarioPool)[i].mState != &dMgJump3DMario_c::StateIdle)
            return 0;
    }
    return 1;
}

void Jump3D_SetLowGravity(void) {
    int i;
    int off;
    i = 0;
    sJump3DGravityStep = 0x100;
    sJump3DFallSpeed = -0x4800;
    if (sJump3DMarioCount <= 0)
        return;
    off = 0;
    do {
        ((dMgJump3DMario_c *)(sJump3DMarioPool + off))->StartFallOut();
        ((dMgJump3DMario_c *)(sJump3DMarioPool + off))->UpdateModelMatrix();
        i++;
        off += 0xb8;
    } while (i < sJump3DMarioCount);
}

void Jump3D_StopGravity(void) {
    int n = sJump3DMarioCount;
    int i = 0;
    int off;
    sJump3DGravityStep = 0;
    if (n <= 0)
    {
        return;
    }
    off = 0;
    do
    {
        ((dMgJump3DMario_c *)(sJump3DMarioPool + off))->StartRiseOut();
        ((dMgJump3DMario_c *)(sJump3DMarioPool + off))->UpdateModelMatrix();
        i++;
        off += 0xb8;
    } while (i < sJump3DMarioCount);
}

/* One Mario's start in a round layout: three per layout (0x24 bytes). */
struct Jump3DLayoutSlot {
    int windUpTimer; /* 0x00: StartWindUp's timer */
    int x;           /* 0x04: start mPos.x */
    int vxScale;     /* 0x08: 20.12 factor on sJump3DStartVx, giving mVel.x */
};
typedef char Jump3DLayoutSlot_size_must_be_0xc[sizeof(Jump3DLayoutSlot) == 0xc ? 1 : -1];

void Jump3D_StartRoundLayout(void) {
    int i;
    int eoff;
    int ooff;

    i = 0;
    if (sJump3DMarioCount <= 0)
        return;
    eoff = 0;
    ooff = 0;
    do {
        Jump3DLayoutSlot *e =
            (Jump3DLayoutSlot *)(sJump3DRoundLayouts + sJump3DLayout * 0x24 + eoff);
        s16 windUp = e->windUpTimer;
        ((dMgJump3DMario_c *)(sJump3DMarioPool + ooff))->StartWindUp(windUp);
        ((dMgJump3DMario_c *)(sJump3DMarioPool + ooff))->mPos.x = e->x;
        {
            int s = sJump3DStartVx;
            int c = e->vxScale;
            ((dMgJump3DMario_c *)(sJump3DMarioPool + ooff))->mVel.x = FX_MUL(s, c);
        }
        ((dMgJump3DMario_c *)(sJump3DMarioPool + ooff))->UpdateModelMatrix();
        i++;
        eoff += 0xc;
        ooff += 0xb8;
    } while (i < sJump3DMarioCount);
}

extern "C" int Jump3D_InitPool(char *base, int count)
{
    int i;
    int off;
    sJump3DMarioPool = base;
    sJump3DMarioCount = count;
    sJump3DMariosLeft = count;
    i = 0;
    if (count > 0) {
        off = i;
        do {
            char *q;
            if (!((dMgJump3DMario_c *)(sJump3DMarioPool + off))->LoadResources())
                return 0;
            ((dMgJump3DMario_c *)(sJump3DMarioPool + off))->HideAndIdle();
            ((dMgJump3DMario_c *)(sJump3DMarioPool + off))->UpdateModelMatrix();
            q = sJump3DMarioPool + off;
            if (i < 3)
                sJumpAdapters[i] = q;
            i++;
            off += 0xb8;
        } while (i < sJump3DMarioCount);
    }
    sJump3DFallSpeed = -0x1e00;
    return 1;
}

/* Vtable slot 2: 1 once Mario has left StateIdle and StateWindUp. */
int dMgJump3DMario_c::IsLaunched() {
    int r = 0;
    int m = 1;

    if (mState == &dMgJump3DMario_c::StateIdle)
        m = 0;
    if (m != 0) {
        m = 1;
        if (mState == &dMgJump3DMario_c::StateWindUp)
            m = 0;
        if (m != 0)
            r = 1;
    }
    return r;
}

/* Vtable slot 1. */
void *dMgJump3DMario_c::GetVelocity() {
    return &mVel;
}

/* Vtable slot 0, and the key function. */
void *dMgJump3DMario_c::GetPos() {
    return &mPos;
}

/* Rebuilds the model matrix from mPos and mAngleY. */
void dMgJump3DMario_c::UpdateModelMatrix()
{
    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x, mPos.y, mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&gScratchMtx, mAngleY);
    mAnmModel.mat4x3 = gScratchMtx;
}

/* OAM shadow sprite plus the model render. */
void dMgJump3DMario_c::Draw()
{
    short screen[2];
    int scaleX;
    int scaleY;
    int wave;
    Matrix2x2 mtx;

    if (mVisible == 0)
        return;

    if (data_ov006_02140400 != 0) {
        Camera_ProjectToScreen16(sMgCurCamera, &mPos, screen);

        wave = FX_SinIdx(sJump3DSpinAngle >> 4);
        screen[1] = screen[1] - (((wave << 2) + 0x30000) >> 12);
        scaleX = FX_Inv((wave >> 2) + 0x1000);

        scaleY = -FX_Inv((FX_CosIdx(sJump3DSpinAngle >> 4) >> 2) + 0x1000);

        /* Zero it through `int *`: writing mtx.a..mtx.d instead costs 11 words
           (notes/experiments/jump3d-2711-mtx-intptr-zeroing.md). */
        int *mp = (int *)&mtx;
        mp[0] = 0; mp[1] = 0; mp[2] = 0; mp[3] = 0;
        mtx.m11 = scaleY;
        mtx.m00 = scaleX;
        OAM::Render(false, data_ov006_02134d1c, screen[0], screen[1], -1, -1, &mtx);
    }

    mAnmModel.Render(&sJump3DMarioScale);
}

/* The per-frame update: gravity, integrate, dispatch mState. */
void dMgJump3DMario_c::Update()
{
    ApproachLinear(mVel.y, sJump3DFallSpeed, sJump3DGravityStep);
    VEC_Add(&mPos, &mVel, &mPos);
    (this->*mState)();
    Camera_ProjectToScreen16(sMgMainCamera, &mPos, &mScreenX);
    UpdateModelMatrix();
    mAnmModel.dExtFrameCtrl_c::Advance();
}

void dMgJump3DMario_c::StateDamp()
{
    mTimer -= 1;
    if (mTimer == 0) {
        ApproachLinear(sJump3DMariosLeft, 0, 1);
        Sound::PlayBank2_2D(0x130);
        Puff_Spawn(mScreenX, 0xc0);
        EnterRespawn();
        return;
    }
    mVel.x = FX_MUL(mVel.x, 0xc00);
    if (mPos.x < -kWallX) {
        mPos.x = -kWallX;
        return;
    }
    if (mPos.x > kWallX)
        mPos.x = kWallX;
}


void dMgJump3DMario_c::EnterDamp()
{
    if (sJump3DMariosLeft > 1)
        Sound::PlayBank2_2D(0x1ca);
    else
        Sound::PlayBank2_2D(0x1c9);
    mAnmModel.SetAnim(sJump3DAnimStruggle, 0, kAnimSpeed, 0);
    mTimer = 40;
    mState = &dMgJump3DMario_c::StateDamp;
}


void dMgJump3DMario_c::StateHold()
{
    u32 idx = gActivePlayerSlot;
    int flag = 0;

    if (gTouchInfo[idx].held != 0) {
        if (gTouchInfo[idx].edge != 0) {
            flag = 1;
        }
    }
    if (flag != 0) {
        /* Keep reusing `flag` here: reading the table straight into `b`
           costs 5 words (notes/experiments/jump3d-2711-statehold-flag-reuse.md). */
        flag = gTouchInfo[idx].y;
        int b = flag;
        int x = mScreenY - 0x20;
        int dz = mScreenX - gTouchInfo[idx].x;
        if (dz < 0) {
            dz = -dz;
        }
        if (dz < 0x18) {
            int dx = x - b;
            if (dx < 0) {
                dx = -dx;
            }
            if (dx < 0x26) {
                ApproachLinear2(mTimer, 0, 8);
            }
        }
    }
    {
        int v = mPos.x;
        if (v < -kWallX) {
            v = -kWallX;
        } else if (v > kWallX) {
            v = kWallX;
        }
        mPos.x = v;
    }
    if (mCommand == 1) {
        mCommand = 0;
        mVel.y = sJump3DBounceVy;
        EnterHit();
    } else if (ApproachLinear2(mTimer, 0, 1)) {
        mCommand = 0;
        if (mVel.y > 0) {
            EnterBounce();
            StateBounce();
        } else {
            EnterMove();
            StateMove();
        }
    } else {
        if (mScreenY < kScreenYMax) {
            return;
        }
        EnterDamp();
    }
}


void dMgJump3DMario_c::EnterHold()
{
    Vector3 v;

    Vec3_Sub(&v, &mPos, &mAnchor);
    if (NormalizeVec3IfNonZero(&v) != 0) {
        mVel.x = v.x;
        mVel.y = v.y;
        mVel.z = v.z;
    } else {
        mVel.x = -mVel.x;
        mVel.y = -mVel.y;
    }
    mAnmModel.SetAnim(sJump3DAnimSdown, 0x40000000, kAnimSpeed, 0);
    mAnmModel.dExtFrameCtrl_c::currFrame = 0;
    Sound_PlayBank1Panned(0, 6, mPos.x);
    mTimer = 32;
    mState = &dMgJump3DMario_c::StateHold;
}


void dMgJump3DMario_c::StateMove()
{
    u16 st = mCommand;

    if (st == 1) {
        mCommand = 0;
        mVel.y = sJump3DBounceVy;
        if (mAnimIdx != 0)
            mAnimIdx = 0;
        else
            mAnimIdx = 1;
        EnterHit();
        return;
    }
    if (st == 2) {
        mCommand = 0;
        EnterHold();
        return;
    }

    {
        int flag = 0;
        u8 idx = gActivePlayerSlot;
        if (gTouchInfo[idx].held != 0 && gTouchInfo[idx].edge != 0)
            flag = 1;

        if (flag != 0) {
            s16 y = mScreenY;
            int ax = gTouchInfo[idx].x;
            int az = gTouchInfo[idx].y;
            int dx = mScreenX - ax;
            int ym = (int)y - 0x20;
            int t = dx < 0 ? -dx : dx;

            if (t < 0x18) {
                t = ym - az;
                if (t < 0) t = -t;
                if (t < 0x26 && (int)y < kScreenYMax) {
                    Vector3 v;
                    v.x = (ax - 0x80) << 12;
                    v.y = (-az) << 12;
                    v.z = 0;
                    mVel.y = sJump3DRiseVy;
                    mVel.x = sJump3DMoveSpeed * dx;
                    if (mAnimIdx != 0)
                        mAnimIdx = 0;
                    else
                        mAnimIdx = 1;
                    mAnmModel.SetAnim((*sJump3DAnimTable[mAnimIdx]), 0x40000000, kAnimSpeed, 0);
                    mAnmModel.dExtFrameCtrl_c::currFrame = 0;
                    Particle::System::NewSimple(kPtclMgJump3DJumpA, mPos.x << 3, mPos.y << 3,
                                                mPos.z << 3);
                    Particle::System::NewSimple(kPtclMgJump3DJumpB, v.x << 3, v.y << 3, v.z << 3);
                    Particle::System::NewSimple(kPtclMgJump3DJumpC, v.x << 3, v.y << 3, v.z << 3);
                    Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
                    func_02012718(0x1c6, mScreenX << 12);
                    EnterBounce();
                }
            }
        }
    }

    if (mScreenY > kScreenYMax) {
        EnterDamp();
        return;
    }

    {
        int pos = mPos.x;
        if (pos < -kWallX && mVel.x < 0) {
            mVel.x = -FX_MUL(mVel.x, 0xd00);
            if (mVel.y > 0) {
                mAnimIdx = 2;
                mAnmModel.SetAnim((*sJump3DAnimTable[mAnimIdx]), 0x40000000, kAnimSpeed, 0);
                mAnmModel.dExtFrameCtrl_c::currFrame = 0;
                Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
            }
        } else if (pos > kWallX && mVel.x > 0) {
            mVel.x = -FX_MUL(mVel.x, 0xd00);
            if (mVel.y > 0) {
                mAnimIdx = 2;
                mAnmModel.SetAnim((*sJump3DAnimTable[mAnimIdx]), 0x40000000, kAnimSpeed, 0);
                mAnmModel.dExtFrameCtrl_c::currFrame = 0;
                Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
            }
        }
    }

    if (mVel.x > 0) {
        UpdateAngle(mAngleY, 0x2800, 2, 0x1000);
        return;
    }
    UpdateAngle(mAngleY, -0x2800, 2, 0x1000);
}


void dMgJump3DMario_c::EnterMove()
{
    mState = &dMgJump3DMario_c::StateMove;
}


void dMgJump3DMario_c::StateFallOut()
{
    if (mPos.y >= -0x120000)
        return;
    mVel.y = 0;
    HideAndIdle();
}


void dMgJump3DMario_c::StartFallOut()
{
    if (mState == &dMgJump3DMario_c::StateRespawn) {
        mVel.y = 0;
        HideAndIdle();
    } else {
        mVel.x = 0;
        mVel.y = 0x2000;
        Sound::PlayBank2_2D(0x1c9);
        mAnmModel.SetAnim(sJump3DAnimStruggle, 0, kAnimSpeed, 0);
        mAnmModel.dExtFrameCtrl_c::currFrame = 0;
        mState = &dMgJump3DMario_c::StateFallOut;
    }
}

void dMgJump3DMario_c::StateRiseOut()
{
    if (mAnmModel.file == sJump3DAnimRollJump) {
        if (mAnmModel.dExtFrameCtrl_c::WillHitFrame(0xc) ||
            mAnmModel.dExtFrameCtrl_c::WillHitFrame(0x18))
            D3D_PlaySeAtX(0x1b5, mPos.x);
    }
    if (mPos.y <= 0x180000)
        return;
    mVel.y = 0;
    sJump3DRisenOutCount++;
    HideAndIdle();
}


void dMgJump3DMario_c::StartRiseOut()
{
    mVel.x = 0;
    mVel.y = sJump3DRiseVy;
    mAnmModel.SetAnim(sJump3DAnimRollJump, 0x40000000, kAnimSpeed, 0);
    mAnmModel.dExtFrameCtrl_c::currFrame = 0;
    Sound::PlayBank2_2D(0x10f);
    func_02012718(0x1b5, mScreenX << 12);
    mState = &dMgJump3DMario_c::StateRiseOut;
}

void dMgJump3DMario_c::EnterHit()
{
    mAnmModel.SetAnim(sJump3DAnimRollJump, 0x40000000, kAnimSpeed, 0);
    mAnmModel.dExtFrameCtrl_c::currFrame = 0;
    if (mState == &dMgJump3DMario_c::StateWindUp) {
        func_02012718(0x110, mScreenX << 12);
    } else {
        Sound_PlayBank1Panned(0, 4, mPos.x);
    }
    func_02012718(0x1b5, mScreenX << 12);
    mState = &dMgJump3DMario_c::StateBounce;
}


void dMgJump3DMario_c::StateBounce()
{
    if (mVel.y < 0) {
        EnterMove();
        StateMove();
        return;
    }

    if (mAnmModel.file == sJump3DAnimRollJump) {
        if (mAnmModel.dExtFrameCtrl_c::WillHitFrame(0xc) != 0 ||
            mAnmModel.dExtFrameCtrl_c::WillHitFrame(0x18) != 0)
            D3D_PlaySeAtX(0x1b5, mPos.x);
    }

    if (mPos.x < -kWallX && mVel.x < 0) {
        mVel.x = -FX_MUL(mVel.x, 0xd00);
        if (mVel.y > 0) {
            mAnimIdx = 2;
            mAnmModel.SetAnim((*sJump3DAnimTable[mAnimIdx]), 0x40000000, kAnimSpeed, 0);
            mAnmModel.dExtFrameCtrl_c::currFrame = 0;
            Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
        }
    } else if (mPos.x > kWallX && mVel.x > 0) {
        mVel.x = -FX_MUL(mVel.x, 0xd00);
        if (mVel.y > 0) {
            mAnimIdx = 2;
            mAnmModel.SetAnim((*sJump3DAnimTable[mAnimIdx]), 0x40000000, kAnimSpeed, 0);
            mAnmModel.dExtFrameCtrl_c::currFrame = 0;
            Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
        }
    } else {
        u16 t = mCommand;
        if (t == 1) {
            mCommand = 0;
            mVel.y = sJump3DBounceVy;
            EnterHit();
            return;
        }
        if (t == 2) {
            mCommand = 0;
            EnterHold();
            return;
        }
    }

    if (mVel.x > 0)
        UpdateAngle(mAngleY, 0x2800, 2, 0x1000);
    else
        UpdateAngle(mAngleY, -0x2800, 2, 0x1000);
}


void dMgJump3DMario_c::EnterBounce()
{
    mState = &dMgJump3DMario_c::StateBounce;
}


void dMgJump3DMario_c::StateWindUp()
{
    if (ApproachLinear2(mTimer, 0, 1) == 0) {
        mPos.y = 0;
        return;
    }
    mVisible = 1;
    mPos.y = 0;
    mVel.x = mVel.x << 1;
    mVel.y = sJump3DRiseVy;
    mCommand = 0;
    EnterHit();
}


void dMgJump3DMario_c::StartWindUp(int timer)
{
    mTimer = timer;
    mState = &dMgJump3DMario_c::StateWindUp;
}

void dMgJump3DMario_c::StateIdle()
{
    mPos.y = 0;
}


void dMgJump3DMario_c::HideAndIdle()
{
    mVisible = 0;
    mPos.y = 0;
    mState = &dMgJump3DMario_c::StateIdle;
}

void dMgJump3DMario_c::StateRespawn()
{
    fx32 rnd;
    int speed;

    mTimer -= 1;
    if (mTimer == 0) {
        mPos.y = 0x100000;
        rnd = RAND_FX();
        mPos.x = (rnd - 0x800) * 0xc0;
        mVel.y = 0;
        speed = sJump3DMoveSpeed;
        rnd = RAND_FX();
        mVel.x = FX_MUL((rnd - 0x800) << 1, speed);
        mAnmModel.SetAnim(sJump3DAnimWallJump, 0x40000000, kAnimSpeed, 0);
        EnterMove();
        return;
    }
    mPos.y = 0x100000;
    mVel.y = 0;
}


void dMgJump3DMario_c::EnterRespawn()
{
    mTimer = 60;
    mPos.y = 0x100000;
    mVel.y = 0;
    mAnmModel.SetAnim(sJump3DAnimWallJump, 0x40000000, kAnimSpeed, 0);
    mState = &dMgJump3DMario_c::StateRespawn;
}


/* Loads the model and the six anims. */
int dMgJump3DMario_c::LoadResources()
{
    /* Keep `t`: folding the comparison into the `if` below shrinks
       the function to 0x160 bytes against the ROM's 0x16c
       (notes/experiments/jump3d-2711-87d0-int-temp.md). */
    int t;

    if (gMarioModelMgModelFile.func_020179b4(&mAnmModel, 1) == 0)
        return 0;

    sJump3DAnimJump = dExtFrameCtrl_c::LoadFile(sJump3DMarioJumpAnimFile);
    sJump3DAnimDoubleJump1 = dExtFrameCtrl_c::LoadFile(sJump3DMarioDoubleJump1AnimFile);
    sJump3DAnimRollJump = dExtFrameCtrl_c::LoadFile(sJump3DMarioRollJumpAnimFile);
    sJump3DAnimWallJump = dExtFrameCtrl_c::LoadFile(sJump3DMarioWallJumpAnimFile);
    sJump3DAnimSdown = dExtFrameCtrl_c::LoadFile(sJump3DMarioSdownAnimFile);
    sJump3DAnimStruggle = dExtFrameCtrl_c::LoadFile(sJump3DMarioStruggleAnimFile);

    mAnimIdx = 0;
    if (sMgMainCamera != 0)
        Camera_ProjectToScreen16(sMgMainCamera, &mPos, &mScreenX);

    t = gCurrentScene->actorID == 0x175;
    if (t != 0) {
        mAnmModel.func_02016a14(0x7fff);
        mAnmModel.func_02016a04(0x210);
    }
    mAnmModel.SetAnim(sJump3DAnimJump, 0x40000000, kAnimSpeed, 0);

    HideAndIdle();
    return 1;
}

dMgJump3DMario_c::~dMgJump3DMario_c()
{
    gMarioModelMgModelFile.Release();
    sJump3DMarioJumpAnimFile.Release();
    sJump3DMarioDoubleJump1AnimFile.Release();
    sJump3DMarioRollJumpAnimFile.Release();
    sJump3DMarioWallJumpAnimFile.Release();
    sJump3DMarioSdownAnimFile.Release();
    sJump3DMarioStruggleAnimFile.Release();
    sJump3DAnimJump = 0;
    sJump3DAnimDoubleJump1 = 0;
    sJump3DAnimWallJump = 0;
    sJump3DAnimSdown = 0;
    sJump3DAnimStruggle = 0;
}


/* Empty: the base constructor, the vptr store and the dExtAnmModel_c member
 * construction are all generated. */
dMgJump3DMario_c::dMgJump3DMario_c()
{
}

Jump3DMarioAnimFilePtr sJump3DMarioJumpAnimFile(32888);
Jump3DMarioAnimFilePtr sJump3DMarioDoubleJump1AnimFile(32797);
Jump3DMarioAnimFilePtr sJump3DMarioRollJumpAnimFile(32910);
Jump3DMarioAnimFilePtr sJump3DMarioWallJumpAnimFile(32958);
Jump3DMarioAnimFilePtr sJump3DMarioSdownAnimFile(32914);
Jump3DMarioAnimFilePtr sJump3DMarioStruggleAnimFile(32932);
Jump3DModelFilePtr gMarioModelMgModelFile(534);

/* The one sprite record Draw hands OAM::Render for the shadow; attr3 == 0xffff
   ends the list. */
OamAttr data_ov006_02134d20[1] = {
    {0x00f8, 0x41f8, 0x001e, 0xffff},
};
