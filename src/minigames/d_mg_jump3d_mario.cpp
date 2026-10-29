//cpp
/* dMgJump3DMario_c: one of the three Marios the two jump minigames
 * (Bounce and Pounce, MG_JUMP and MG_JUMP2) hold as mPlayers[3].
 *
 * A pointer-to-member state machine: mState is installed from the 8-byte
 * records at pmf_dMgJump3DMario_c_StateMove..0213b098. Each update applies gravity,
 * integrates mVel into mPos, runs the current state and re-projects the
 * screen position. Tap near Mario and he bounces; miss and he damps out and
 * respawns.
 *
 * Source order is ROM order, which `#pragma defer_codegen off` keeps; do not
 * reorder. The destructor is not virtual, so the first declared virtual
 * (GetPos) is the key function and this file emits the vtable.
 *
 * Include common.h first: the mat4x3 copy in UpdateModelMatrix needs its
 * flat s32 m[12]; math/Matrix.h's nested form scalarizes the copy.
 * decl_common.h is left out because four of the data symbols here collide
 * with its spellings.
 *
 * ov006 0x020c6f8c..0x020c8a30 (.text), 40 functions. The first twelve are
 * the helpers the owning scenes call: a tuning-word rescale
 * (Jump3D_SetTuning), the draw/update/reset/init loops over the
 * three-Mario array and its small state accessors (0x020c70d0..0x020c7574).
 * They share the .data tuning words at 0213b008..0213b01c and the bss
 * window at 02140400.. with the members below, which is what puts them in
 * this TU. They were one-function shards, one per function; Jump3D_StartRoundLayout keeps its
 * `opt_common_subs off` as a push/pop bracket.
 *
 * leftovers:
 * - dExtAnmModel_c::SetAnim takes Fix12<int> by value; its real spelling makes
 *   the compiler home the argument and the size changes. It stays mangled.
 * - The helper functions and data_ov006_ tables have no header or name yet.
 * - The five gTouch-lane and gActivePlayerSlot externs keep the file-local record
 *   spellings (u8 [][4] lanes) because the ROM scales the slot in the
 *   addressing mode, which conflicts with PlayerInput.h's declarations;
 *   that header stays unincluded and the sites are marked `local extern`.
 * - The *(volatile int *)&v[2] store in StateMove, the int* zeroing of
 *   Mtx in Draw, and the `int t` temp in LoadResources
 *   are codegen-pinned; each site carries its measured note.
 */

#pragma defer_codegen off

#include "common.h"
#include "dMgJump3DMario_c.h"
#include "SharedFilePtr.h"
#include "OAM.h"
#include "fBase_c.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"

static const int kWallX = 0x6c000;   /* mPos.x clamp: the arena walls */
static const int kScreenYMax = 0xbc; /* past this mScreenY the Mario damps out */
static const int kAnimSpeed = 0x800; /* SetAnim speed every install uses */

/* OAM.h forward-declares Matrix2x2 and says the files that pass one keep
 * their own 4 x s32 definition; this is that definition. */
struct Mtx { int a, b, c, d; };

/* Everything this file calls or reads that no header declares. Keep it
 * above the first function. */
extern "C" {

/* Fix12<int> by value in the real signatures; see the banner. */
namespace Particle {
struct System {
    static void NewSimple(unsigned int id, int x, int y, int z);
};
}
void Sound_PlayBank1Panned(int a, int b, int c);
void Vec3_Sub(Jump3DVec *out, Jump3DVec *a, Jump3DVec *b);
void AddVec3(Jump3DVec *a, Jump3DVec *b, Jump3DVec *c);
int  NormalizeVec3IfNonZero(Jump3DVec *v);
int  RandomIntInternal(int *seed);
void func_02012718(int a, int b);
int  func_02053200(int x);
void func_02016a14(void *self, int a);
void func_02016a04(void *self, int a);
int func_020179b4(SharedFilePtr *f, dExtModel_c *m, int extra);
void Camera_ProjectToScreen16(void *cam, Jump3DVec *pos, short *out);
void D3D_PlaySeAtX(int a, int b);
void Puff_Spawn(int a, int b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
int  func_020531a4(int a);
void Puff_DrawAll(void);
void Puff_UpdateAll(void);
void Puff_ClearAll(int a, int b);

/* The mState records this file installs or compares. */
extern Jump3DState pmf_dMgJump3DMario_c_StateMove;
extern Jump3DState pmf_dMgJump3DMario_c_StateHold;
extern Jump3DState pmf_dMgJump3DMario_c_StateDamp;
extern Jump3DState pmf_dMgJump3DMario_c_StateRespawn;
extern Jump3DState pmf_dMgJump3DMario_c_StateIdle_2;
extern Jump3DState pmf_dMgJump3DMario_c_StateWindUp;
extern Jump3DState pmf_dMgJump3DMario_c_StateIdle_3;
extern Jump3DState pmf_dMgJump3DMario_c_StateBounce;
extern Jump3DState pmf_dMgJump3DMario_c_StateWindUp_2;
extern Jump3DState pmf_dMgJump3DMario_c_StateWindUp_3;
extern Jump3DState pmf_dMgJump3DMario_c_StateBounce_2;
extern Jump3DState pmf_dMgJump3DMario_c_StateRiseOut;
extern Jump3DState pmf_dMgJump3DMario_c_StateRespawn_2;
extern Jump3DState pmf_dMgJump3DMario_c_StateFallOut;

/* Slots the current mAnimIdx (0/1/2) picks the anim file from. */
extern BCA_File **sJump3DAnimTable[];

/* Tuning words, read as plain ints. */
extern int sJump3DBounceVy;
extern int sJump3DRiseVy;
extern int sJump3DStartVx;
extern int sJump3DFallSpeed;
extern int sJump3DGravityStep;
extern int sJump3DMoveSpeed;
extern int sJump3DMariosLeft;
extern int sJump3DRisenOutCount;

/* The six anim files LoadResources loads. The destructor clears only
 * five of them; 0214041c survives in the ROM too. */
extern BCA_File *sJump3DAnimSdown;
extern BCA_File *sJump3DAnimDoubleJump1;
extern BCA_File *sJump3DAnimRollJump;
extern BCA_File *sJump3DAnimWallJump;
extern BCA_File *sJump3DAnimStruggle;
extern BCA_File *sJump3DAnimJump;

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
extern int sJump3DArrangeLevel;
extern int sJump3DLayout;
extern int sJump3DMarioCount;
extern char *sJump3DMarioPool;
extern char *sJumpAdapters[];
extern int pmf_dMgJump3DMario_c_StateIdle[2];

extern u8  data_ov006_02140400;
extern u16 sJump3DSpinAngle;
extern void *sMgMainCamera;
extern void *sMgCurCamera;
extern OamAttr *data_ov006_02134d1c;
extern Vector3 sJump3DMarioScale;
extern char sJump3DRoundLayouts[];

// local extern: this file needs a record-view spelling of one of the touch lanes (the ROM scales the slot in the addressing mode), which conflicts with PlayerInput.h; the header is not included and all five symbols are declared here.
extern u8 gActivePlayerSlot;
// local extern: see above.
extern u8 gTouchHeld[][4];
// local extern: see above.
extern u8 gTouchEdge[][4];
// local extern: see above.
extern u8 gTouchX[][4];
// local extern: see above.
extern u8 gTouchY[][4];
extern Matrix4x3 data_020a0e68;
extern short data_02082214[];
extern int data_0209e650;
extern fBase_c *data_0209f5c0;

}  /* extern "C" */



// @symbol Jump3D_SetTuning
/* Rescales the six tuning words at 0213b008..0213b01c from one input,
 * clamped to 0x3000. */
extern "C" void Jump3D_SetTuning(int a)
{
    int b;
    if (a > 0x3000) a = 0x3000;
    b = func_020531a4(a);
    sJump3DFallSpeed = FX_MUL(b, -0x2400);
    sJump3DGravityStep = FX_MUL(a, 0xe0);
    sJump3DRiseVy = FX_MUL(b, 0x4c00);
    sJump3DMoveSpeed = FX_MUL(b, 0x200);
    sJump3DStartVx = FX_MUL(b, 0x180);
    sJump3DBounceVy = FX_MUL(b, 0x5000);
}

// @symbol Jump3D_DrawAll
extern "C" void Jump3D_DrawAll(void)
{
    int i;
    for (i = 0; i < sJump3DMarioCount; i++) {
        ((dMgJump3DMario_c *)(sJump3DMarioPool + i * 0xb8))->Draw();
    }
    Puff_DrawAll();
}

// @symbol Jump3D_UpdateAll
extern "C" void Jump3D_UpdateAll(void)
{
    int i;
    sJump3DSpinAngle = sJump3DSpinAngle + 0x800;
    for (i = 0; i < sJump3DMarioCount; i++) {
        ((dMgJump3DMario_c *)(sJump3DMarioPool + i * 0xb8))->Update();
    }
    Puff_UpdateAll();
}

// @symbol Jump3D_Arrange
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

// @symbol Jump3D_DisableInput
extern "C" void Jump3D_DisableInput(void) { data_ov006_02140400 = 0; }

// @symbol Jump3D_EnableInput
extern "C" void Jump3D_EnableInput(void) { data_ov006_02140400 = 1; }

// @symbol Jump3D_RestoreCount
extern "C" void Jump3D_RestoreCount(void)
{
    ApproachLinear(sJump3DMariosLeft, sJump3DMarioCount, 1);
}

// @symbol Jump3D_AllIdle
/* Returns 0 if any entry's pair at +0x3c disagrees with the record at
 * 0213b040 (a y mismatch is tolerated when x is 0), else 1. */
struct Jump3DEnt {
    char _pad0[0x3c];
    int x;
    int y;
    char _pad44[0x74];
};
extern "C" int Jump3D_AllIdle(void)
{
    int i;
    for (i = 0; i < sJump3DMarioCount; i++) {
        int *v = (int *)(&((Jump3DEnt *)sJump3DMarioPool)[i].x);
        volatile int *q = (volatile int *)pmf_dMgJump3DMario_c_StateIdle;
        if (v[0] != pmf_dMgJump3DMario_c_StateIdle[0] ||
            (v[1] != q[1] && ((Jump3DEnt *)sJump3DMarioPool)[i].x != 0))
            return 0;
    }
    return 1;
}

// @symbol Jump3D_SetLowGravity
extern "C" void Jump3D_SetLowGravity(void)
{
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

// @symbol Jump3D_StopGravity
extern "C" void Jump3D_StopGravity(void)
{
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

// @symbol Jump3D_StartRoundLayout
#pragma push
#pragma opt_common_subs off
extern "C" void Jump3D_StartRoundLayout(void)
{
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
        int word = e->windUpTimer;
        ((dMgJump3DMario_c *)(sJump3DMarioPool + ooff))->StartWindUp((short)word);
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
#pragma pop

// @symbol Jump3D_InitPool
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

// @symbol _ZN16dMgJump3DMario_c10IsLaunchedEv
/* Vtable slot 2: 1 once Mario has left StateIdle and StateWindUp. */
int dMgJump3DMario_c::IsLaunched() {
    int r = 0;
    int m = 1;

    if (mState == pmf_dMgJump3DMario_c_StateIdle_3)
        m = 0;
    if (m != 0) {
        m = 1;
        if (mState == pmf_dMgJump3DMario_c_StateWindUp_3)
            m = 0;
        if (m != 0)
            r = 1;
    }
    return r;
}

// @symbol _ZN16dMgJump3DMario_c11GetVelocityEv
/* Vtable slot 1. */
void *dMgJump3DMario_c::GetVelocity() {
    return &mVel;
}

// @symbol _ZN16dMgJump3DMario_c6GetPosEv
/* Vtable slot 0, and the key function. */
void *dMgJump3DMario_c::GetPos() {
    return &mPos;
}

// @symbol _ZN16dMgJump3DMario_c17UpdateModelMatrixEv
/* Rebuilds the model matrix from mPos and mAngleY. */
void dMgJump3DMario_c::UpdateModelMatrix()
{
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x, mPos.y, mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    mAnmModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN16dMgJump3DMario_c4DrawEv
/* OAM shadow sprite plus the model render. */
void dMgJump3DMario_c::Draw()
{
    short screen[2];
    int scaleX;
    int scaleY;
    int angle;
    int wave;
    Mtx mtx;

    if (mVisible == 0)
        return;

    if (data_ov006_02140400 != 0) {
        Camera_ProjectToScreen16(sMgCurCamera, &mPos, screen);

        angle = sJump3DSpinAngle;
        wave = data_02082214[(angle >> 4) * 2];
        screen[1] = screen[1] - (((wave << 2) + 0x30000) >> 12);
        scaleX = func_02053200((wave >> 2) + 0x1000);

        angle = sJump3DSpinAngle;
        scaleY = -func_02053200((data_02082214[(angle >> 4) * 2 + 1] >> 2) + 0x1000);

        /* Zero it through `int *`: writing mtx.a..mtx.d instead costs 11 words
           (notes/experiments/jump3d-2711-mtx-intptr-zeroing.md). */
        int *mp = (int *)&mtx;
        mp[0] = 0; mp[1] = 0; mp[2] = 0; mp[3] = 0;
        mtx.d = scaleY;
        mtx.a = scaleX;
        OAM::Render(false, data_ov006_02134d1c, screen[0], screen[1], -1, -1, (Matrix2x2 *)&mtx);
    }

    mAnmModel.Render(&sJump3DMarioScale);
}

// @symbol _ZN16dMgJump3DMario_c6UpdateEv
/* The per-frame update: gravity, integrate, dispatch mState. */
void dMgJump3DMario_c::Update()
{
    ApproachLinear(mVel.y, sJump3DFallSpeed, sJump3DGravityStep);
    AddVec3(&mPos, &mVel, &mPos);
    (this->*mState)();
    Camera_ProjectToScreen16(sMgMainCamera, &mPos, &mScreenX);
    UpdateModelMatrix();
    mAnmModel.dExtFrameCtrl_c::Advance();
}

// @symbol _ZN16dMgJump3DMario_c9StateDampEv
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


// @symbol _ZN16dMgJump3DMario_c9EnterDampEv
void dMgJump3DMario_c::EnterDamp()
{
    if (sJump3DMariosLeft > 1)
        Sound::PlayBank2_2D(0x1ca);
    else
        Sound::PlayBank2_2D(0x1c9);
    mAnmModel.SetAnim((BCA_File *)sJump3DAnimStruggle, 0, kAnimSpeed, 0);
    mTimer = 0x28;
    mState = pmf_dMgJump3DMario_c_StateDamp;
}


// @symbol _ZN16dMgJump3DMario_c9StateHoldEv
void dMgJump3DMario_c::StateHold()
{
    u32 idx = gActivePlayerSlot;
    int flag = 0;

    if (gTouchHeld[idx][0] != 0) {
        if (gTouchEdge[idx][0] != 0) {
            flag = 1;
        }
    }
    if (flag != 0) {
        /* Keep reusing `flag` here: reading the table straight into `b`
           costs 5 words (notes/experiments/jump3d-2711-statehold-flag-reuse.md). */
        flag = gTouchY[idx][0];
        int b = flag;
        int x = mScreenY - 0x20;
        int dz = mScreenX - gTouchX[idx][0];
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


// @symbol _ZN16dMgJump3DMario_c9EnterHoldEv
void dMgJump3DMario_c::EnterHold()
{
    /* Jump3DVec, not Vector3: Vector3 matches here too, but it makes this
       file emit Vector3's destructor, which the manifest does not license
       (notes/experiments/jump3d-2711-scratch-vector3.md). */
    Jump3DVec v;

    Vec3_Sub(&v, &mPos, &mAnchor);
    if (NormalizeVec3IfNonZero(&v) != 0) {
        mVel.x = v.x;
        mVel.y = v.y;
        mVel.z = v.z;
    } else {
        mVel.x = -mVel.x;
        mVel.y = -mVel.y;
    }
    mAnmModel.SetAnim((BCA_File *)sJump3DAnimSdown, 0x40000000, kAnimSpeed, 0);
    mAnmModel.dExtFrameCtrl_c::currFrame = 0;
    Sound_PlayBank1Panned(0, 6, mPos.x);
    mTimer = 0x20;
    mState = pmf_dMgJump3DMario_c_StateHold;
}


// @symbol _ZN16dMgJump3DMario_c9StateMoveEv
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
        if (gTouchHeld[idx][0] != 0 && gTouchEdge[idx][0] != 0)
            flag = 1;

        if (flag != 0) {
            s16 y = mScreenY;
            int ax = (int)gTouchX[idx][0];
            int az = (int)gTouchY[idx][0];
            int dx = (int)mScreenX - ax;
            int ym = (int)y - 0x20;
            int t = dx < 0 ? -dx : dx;

            if (t < 0x18) {
                t = ym - az;
                if (t < 0) t = -t;
                if (t < 0x26 && (int)y < kScreenYMax) {
                    int v[3];
                    int *p = &sJump3DRiseVy;
                    int t0 = (ax - 0x80) << 12;
                    int t1 = (-az) << 12;
                    v[0] = t0;
                    v[1] = t1;
                    /* Keep the volatile store: a plain `v[2] = 0;` shrinks the
                       function to 0x3a8 bytes against the ROM's 0x3c4
                       (notes/experiments/jump3d-2711-statemove-volatile-store.md). */
                    *(volatile int *)&v[2] = 0;
                    mVel.y = *p;
                    mVel.x = sJump3DMoveSpeed * dx;
                    if (mAnimIdx != 0)
                        mAnimIdx = 0;
                    else
                        mAnimIdx = 1;
                    mAnmModel.SetAnim((BCA_File *)(*sJump3DAnimTable[mAnimIdx]), 0x40000000,
                                      kAnimSpeed, 0);
                    mAnmModel.dExtFrameCtrl_c::currFrame = 0;
                    Particle::System::NewSimple(0xed, mPos.x << 3, mPos.y << 3, mPos.z << 3);
                    Particle::System::NewSimple(0xee, v[0] << 3, v[1] << 3, v[2] << 3);
                    Particle::System::NewSimple(0xef, v[0] << 3, v[1] << 3, v[2] << 3);
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
                mAnmModel.SetAnim((BCA_File *)(*sJump3DAnimTable[mAnimIdx]), 0x40000000, kAnimSpeed,
                                  0);
                mAnmModel.dExtFrameCtrl_c::currFrame = 0;
                Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
            }
        } else if (pos > kWallX && mVel.x > 0) {
            mVel.x = -FX_MUL(mVel.x, 0xd00);
            if (mVel.y > 0) {
                mAnimIdx = 2;
                mAnmModel.SetAnim((BCA_File *)(*sJump3DAnimTable[mAnimIdx]), 0x40000000, kAnimSpeed,
                                  0);
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


// @symbol _ZN16dMgJump3DMario_c9EnterMoveEv
void dMgJump3DMario_c::EnterMove()
{
    mState = pmf_dMgJump3DMario_c_StateMove;
}


// @symbol _ZN16dMgJump3DMario_c12StateFallOutEv
void dMgJump3DMario_c::StateFallOut()
{
    if (mPos.y >= -0x120000)
        return;
    mVel.y = 0;
    HideAndIdle();
}


// @symbol _ZN16dMgJump3DMario_c12StartFallOutEv
void dMgJump3DMario_c::StartFallOut()
{
    if (mState == pmf_dMgJump3DMario_c_StateRespawn_2) {
        mVel.y = 0;
        HideAndIdle();
    } else {
        mVel.x = 0;
        mVel.y = 0x2000;
        Sound::PlayBank2_2D(0x1c9);
        mAnmModel.SetAnim((BCA_File *)sJump3DAnimStruggle, 0, kAnimSpeed, 0);
        mAnmModel.dExtFrameCtrl_c::currFrame = 0;
        mState = pmf_dMgJump3DMario_c_StateFallOut;
    }
}

// @symbol _ZN16dMgJump3DMario_c12StateRiseOutEv
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


// @symbol _ZN16dMgJump3DMario_c12StartRiseOutEv
void dMgJump3DMario_c::StartRiseOut()
{
    mVel.x = 0;
    mVel.y = sJump3DRiseVy;
    mAnmModel.SetAnim((BCA_File *)sJump3DAnimRollJump, 0x40000000, kAnimSpeed, 0);
    mAnmModel.dExtFrameCtrl_c::currFrame = 0;
    Sound::PlayBank2_2D(0x10f);
    func_02012718(0x1b5, mScreenX << 12);
    mState = pmf_dMgJump3DMario_c_StateRiseOut;
}

// @symbol _ZN16dMgJump3DMario_c8EnterHitEv
void dMgJump3DMario_c::EnterHit()
{
    mAnmModel.SetAnim((BCA_File *)sJump3DAnimRollJump, 0x40000000, kAnimSpeed, 0);
    mAnmModel.dExtFrameCtrl_c::currFrame = 0;
    if (mState == pmf_dMgJump3DMario_c_StateWindUp_2) {
        func_02012718(0x110, mScreenX << 12);
    } else {
        Sound_PlayBank1Panned(0, 4, mPos.x);
    }
    func_02012718(0x1b5, mScreenX << 12);
    mState = pmf_dMgJump3DMario_c_StateBounce_2;
}


// @symbol _ZN16dMgJump3DMario_c11StateBounceEv
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
            mAnmModel.SetAnim((BCA_File *)(*sJump3DAnimTable[mAnimIdx]), 0x40000000, kAnimSpeed, 0);
            mAnmModel.dExtFrameCtrl_c::currFrame = 0;
            Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
        }
    } else if (mPos.x > kWallX && mVel.x > 0) {
        mVel.x = -FX_MUL(mVel.x, 0xd00);
        if (mVel.y > 0) {
            mAnimIdx = 2;
            mAnmModel.SetAnim((BCA_File *)(*sJump3DAnimTable[mAnimIdx]), 0x40000000, kAnimSpeed, 0);
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


// @symbol _ZN16dMgJump3DMario_c11EnterBounceEv
void dMgJump3DMario_c::EnterBounce()
{
    mState = pmf_dMgJump3DMario_c_StateBounce;
}


// @symbol _ZN16dMgJump3DMario_c11StateWindUpEv
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


// @symbol _ZN16dMgJump3DMario_c11StartWindUpEi
void dMgJump3DMario_c::StartWindUp(int timer)
{
    mTimer = (s16)timer;
    mState = pmf_dMgJump3DMario_c_StateWindUp;
}

// @symbol _ZN16dMgJump3DMario_c9StateIdleEv
void dMgJump3DMario_c::StateIdle()
{
    mPos.y = 0;
}


// @symbol _ZN16dMgJump3DMario_c11HideAndIdleEv
void dMgJump3DMario_c::HideAndIdle()
{
    mVisible = 0;
    mPos.y = 0;
    mState = pmf_dMgJump3DMario_c_StateIdle_2;
}

// @symbol _ZN16dMgJump3DMario_c12StateRespawnEv
void dMgJump3DMario_c::StateRespawn()
{
    unsigned int rnd;
    int speed;

    mTimer -= 1;
    if (mTimer == 0) {
        mPos.y = 0x100000;
        rnd = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        mPos.x = ((int)rnd - 0x800) * 0xc0;
        mVel.y = 0;
        speed = sJump3DMoveSpeed;
        rnd = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        mVel.x = FX_MUL((((int)rnd - 0x800) << 1), speed);
        mAnmModel.SetAnim((BCA_File *)sJump3DAnimWallJump, 0x40000000, kAnimSpeed, 0);
        EnterMove();
        return;
    }
    mPos.y = 0x100000;
    mVel.y = 0;
}


// @symbol _ZN16dMgJump3DMario_c12EnterRespawnEv
void dMgJump3DMario_c::EnterRespawn()
{
    mTimer = 0x3c;
    mPos.y = 0x100000;
    mVel.y = 0;
    mAnmModel.SetAnim((BCA_File *)sJump3DAnimWallJump, 0x40000000, kAnimSpeed, 0);
    mState = pmf_dMgJump3DMario_c_StateRespawn;
}


// @symbol _ZN16dMgJump3DMario_c13LoadResourcesEv
/* Loads the model and the six anims. */
int dMgJump3DMario_c::LoadResources()
{
    /* Keep `t`: folding the comparison into the `if` below shrinks
       the function to 0x160 bytes against the ROM's 0x16c
       (notes/experiments/jump3d-2711-87d0-int-temp.md). */
    int t;

    if (func_020179b4(&gMarioModelMgModelFile, &mAnmModel, 1) == 0)
        return 0;

    sJump3DAnimJump = (BCA_File *)dExtFrameCtrl_c::LoadFile(sJump3DMarioJumpAnimFile);
    sJump3DAnimDoubleJump1 = (BCA_File *)dExtFrameCtrl_c::LoadFile(sJump3DMarioDoubleJump1AnimFile);
    sJump3DAnimRollJump = (BCA_File *)dExtFrameCtrl_c::LoadFile(sJump3DMarioRollJumpAnimFile);
    sJump3DAnimWallJump = (BCA_File *)dExtFrameCtrl_c::LoadFile(sJump3DMarioWallJumpAnimFile);
    sJump3DAnimSdown = (BCA_File *)dExtFrameCtrl_c::LoadFile(sJump3DMarioSdownAnimFile);
    sJump3DAnimStruggle = (BCA_File *)dExtFrameCtrl_c::LoadFile(sJump3DMarioStruggleAnimFile);

    mAnimIdx = 0;
    if (sMgMainCamera != 0)
        Camera_ProjectToScreen16(sMgMainCamera, &mPos, &mScreenX);

    t = data_0209f5c0->actorID == 0x175;
    if (t != 0) {
        func_02016a14(&mAnmModel, 0x7fff);
        func_02016a04(&mAnmModel, 0x210);
    }
    mAnmModel.SetAnim((BCA_File *)sJump3DAnimJump, 0x40000000, kAnimSpeed, 0);

    HideAndIdle();
    return 1;
}

// @symbol _ZN16dMgJump3DMario_cD1Ev
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


// @symbol _ZN16dMgJump3DMario_cC1Ev
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
