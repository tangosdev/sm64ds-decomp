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
 * (Unk_020c76d8) is the key function and this file emits the vtable.
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
 * - ModelAnim::SetAnim takes Fix12<int> by value; its real spelling makes
 *   the compiler home the argument and the size changes. NewSimple takes
 *   Fix12<int> the same way and has no header. Both stay mangled.
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
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a, int b, unsigned int d);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void Sound_PlayBank1Panned(int a, int b, int c);
void Vec3_Sub(Jump3DVec *out, Jump3DVec *a, Jump3DVec *b);
void AddVec3(Jump3DVec *a, Jump3DVec *b, Jump3DVec *c);
int  NormalizeVec3IfNonZero(Jump3DVec *v);
int  RandomIntInternal(int *seed);
void func_02012718(int a, int b);
int  func_02053200(int x);
void func_02016a14(void *self, int a);
void func_02016a04(void *self, int a);
int  func_020179b4(SharedFilePtr *f, ModelBase *m, int extra);
void Camera_ProjectToScreen16(void *cam, Jump3DVec *pos, short *out);
void D3D_PlaySeAtX(int a, int b);
void Puff_Spawn(int a, int b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
int  func_020531a4(int a);
void Puff_DrawAll(void);
void Puff_UpdateAll(void);
void Puff_ClearAll(int a, int b);

/* The mState records this file installs or compares.
 * The unsigned arm is the static image; .pmf is the same bytes read as a
 * pointer-to-member so equality keeps the null short-circuit. */
union Jump3DRec {
    unsigned w[2];
    Jump3DState pmf;
};
extern unsigned pmf_dMgJump3DMario_c_StateMove[2];
extern unsigned pmf_dMgJump3DMario_c_StateHold[2];
extern unsigned pmf_dMgJump3DMario_c_StateDamp[2];
extern unsigned pmf_dMgJump3DMario_c_StateRespawn[2];
extern unsigned pmf_dMgJump3DMario_c_StateIdle[2];
extern unsigned pmf_dMgJump3DMario_c_StateIdle_2[2];
extern unsigned pmf_dMgJump3DMario_c_StateWindUp[2];
extern Jump3DRec pmf_dMgJump3DMario_c_StateIdle_3;
extern unsigned pmf_dMgJump3DMario_c_StateBounce[2];
extern Jump3DRec pmf_dMgJump3DMario_c_StateWindUp_2;
extern Jump3DRec pmf_dMgJump3DMario_c_StateWindUp_3;
extern Jump3DRec pmf_dMgJump3DMario_c_StateBounce_2;
extern unsigned pmf_dMgJump3DMario_c_StateRiseOut[2];
extern Jump3DRec pmf_dMgJump3DMario_c_StateRespawn_2;
extern Jump3DRec pmf_dMgJump3DMario_c_StateFallOut;

/* Slots the current mAnimIdx (0/1/2) picks the anim file from. */
extern BCA_File **data_ov006_0213b098[];

/* Tuning words, read as plain ints. */
extern int data_ov006_0213b008;
extern int data_ov006_0213b00c;
extern int data_ov006_0213b014;
extern int data_ov006_0213b010;
extern int data_ov006_0213b018;
extern int data_ov006_0213b01c;
extern int data_ov006_02140428;
extern int data_ov006_02140434;

/* The six anim files LoadResources loads. The destructor clears only
 * five of them; 0214041c survives in the ROM too. */
extern BCA_File *data_ov006_02140408;
extern BCA_File *data_ov006_0214040c;
extern BCA_File *data_ov006_0214041c;
extern BCA_File *data_ov006_02140424;
extern BCA_File *data_ov006_0214042c;
extern BCA_File *data_ov006_02140430;

/* Six anim handles call SharedFilePtr::Construct. The model handle calls
   func_02017a24. The second word is the loaded file. */
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

extern Jump3DMarioAnimFilePtr data_ov006_02140438;
extern Jump3DMarioAnimFilePtr data_ov006_02140440;
extern Jump3DMarioAnimFilePtr data_ov006_02140448;
extern Jump3DModelFilePtr data_ov006_02140450;
extern Jump3DMarioAnimFilePtr data_ov006_02140458;
extern Jump3DMarioAnimFilePtr data_ov006_02140460;
extern Jump3DMarioAnimFilePtr data_ov006_02140468;

/* The pool the minigame scenes hand in: count, base pointer, level, round
 * and the three player slots. */
extern int data_ov006_02140410;
extern int data_ov006_02140414;
extern int data_ov006_02140418;
extern char *data_ov006_02140420;
extern char *data_ov006_021403f4[];

extern u8  data_ov006_02140400;
extern u16 data_ov006_02140404;
extern void *data_ov006_02141a40;
extern void *data_ov006_02141a44;
extern OamAttr *data_ov006_02134d1c;
/* Three 20.12 words, 0x2000. A Vector3 object would register its destructor
 * and grow this TU's static initializer, so the words stay a const array. */
extern const int data_ov006_0212ddd0[3];
extern char data_ov006_0212dddc[];

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

void  UpdateAngle(short &angle, short target, int speed, short maxStep);
short ApproachLinear2(short &value, short target, short step);
void  ApproachLinear(int &value, int target, int step);


// @symbol Jump3D_SetTuning
/* Rescales the six tuning words at 0213b008..0213b01c from one input,
 * clamped to 0x3000. */
extern "C" void Jump3D_SetTuning(int a)
{
    int b;
    if (a > 0x3000) a = 0x3000;
    b = func_020531a4(a);
    data_ov006_0213b010 = FX_MUL(b, -0x2400);
    data_ov006_0213b018 = FX_MUL(a, 0xe0);
    data_ov006_0213b00c = FX_MUL(b, 0x4c00);
    data_ov006_0213b01c = FX_MUL(b, 0x200);
    data_ov006_0213b014 = FX_MUL(b, 0x180);
    data_ov006_0213b008 = FX_MUL(b, 0x5000);
}

// @symbol Jump3D_DrawAll
extern "C" void Jump3D_DrawAll(void)
{
    int i;
    for (i = 0; i < data_ov006_02140418; i++) {
        ((dMgJump3DMario_c *)(data_ov006_02140420 + i * 0xb8))->Draw();
    }
    Puff_DrawAll();
}

// @symbol Jump3D_UpdateAll
extern "C" void Jump3D_UpdateAll(void)
{
    int i;
    data_ov006_02140404 = data_ov006_02140404 + 0x800;
    for (i = 0; i < data_ov006_02140418; i++) {
        ((dMgJump3DMario_c *)(data_ov006_02140420 + i * 0xb8))->Update();
    }
    Puff_UpdateAll();
}

// @symbol Jump3D_Arrange
extern "C" void Jump3D_Arrange(u32 a0, u32 a1)
{
    int i;
    int x;
    data_ov006_02140410 = a0;
    data_ov006_02140414 = a1;
    if (a0 < 5) {
        x = a0 * 0x140 + 0x1000;
    } else {
        x = (a0 % 5 + a0 / 5) * 0x280 + 0x1000;
    }
    Jump3D_SetTuning(x);
    if ((u32)data_ov006_02140410 % 5 == 0) {
        data_ov006_02140428 = data_ov006_02140418;
    }
    for (i = 0; i < data_ov006_02140418; i++) {
        ((dMgJump3DMario_c *)(data_ov006_02140420 + i * 0xb8))->HideAndIdle();
    }
    Puff_ClearAll(data_ov006_02140418 - data_ov006_02140428, 0);
    data_ov006_02140434 = 0;
}

// @symbol Jump3D_DisableInput
extern "C" void Jump3D_DisableInput(void) { data_ov006_02140400 = 0; }

// @symbol Jump3D_EnableInput
extern "C" void Jump3D_EnableInput(void) { data_ov006_02140400 = 1; }

// @symbol Jump3D_RestoreCount
extern "C" void Jump3D_RestoreCount(void)
{
    ApproachLinear(data_ov006_02140428, data_ov006_02140418, 1);
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
    for (i = 0; i < data_ov006_02140418; i++) {
        int *v = (int *)(&((Jump3DEnt *)data_ov006_02140420)[i].x);
        int *p = (int *)&pmf_dMgJump3DMario_c_StateIdle;
        volatile int *q = (volatile int *)&pmf_dMgJump3DMario_c_StateIdle;
        if (v[0] != p[0]
            || (v[1] != q[1] && ((Jump3DEnt *)data_ov006_02140420)[i].x != 0))
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
    data_ov006_0213b018 = 0x100;
    data_ov006_0213b010 = -0x4800;
    if (data_ov006_02140418 <= 0)
        return;
    off = 0;
    do {
        ((dMgJump3DMario_c *)(data_ov006_02140420 + off))->StartFallOut();
        ((dMgJump3DMario_c *)(data_ov006_02140420 + off))->UpdateModelMatrix();
        i++;
        off += 0xb8;
    } while (i < data_ov006_02140418);
}

// @symbol Jump3D_StopGravity
extern "C" void Jump3D_StopGravity(void)
{
    int n = data_ov006_02140418;
    int i = 0;
    int off;
    data_ov006_0213b018 = 0;
    if (n <= 0)
    {
        return;
    }
    off = 0;
    do
    {
        ((dMgJump3DMario_c *)(data_ov006_02140420 + off))->StartRiseOut();
        ((dMgJump3DMario_c *)(data_ov006_02140420 + off))->UpdateModelMatrix();
        i++;
        off += 0xb8;
    }
    while (i < data_ov006_02140418);
}

// @symbol Jump3D_StartRoundLayout
#pragma push
#pragma opt_common_subs off
extern "C" void Jump3D_StartRoundLayout(void)
{
    int i;
    int eoff;
    int ooff;

    i = 0;
    if (data_ov006_02140418 <= 0)
        return;
    eoff = 0;
    ooff = 0;
    do {
        char *e = data_ov006_0212dddc + data_ov006_02140414 * 0x24 + eoff;
        int word = *(int *)e;
        ((dMgJump3DMario_c *)(data_ov006_02140420 + ooff))->StartWindUp((short)word);
        *(int *)(data_ov006_02140420 + ooff + 0x14) = *(int *)(e + 4);
        {
            int s = data_ov006_0213b014;
            int c = *(int *)(e + 8);
            *(int *)(data_ov006_02140420 + ooff + 0x20) = FX_MUL(s, c);
        }
        ((dMgJump3DMario_c *)(data_ov006_02140420 + ooff))->UpdateModelMatrix();
        i++;
        eoff += 0xc;
        ooff += 0xb8;
    } while (i < data_ov006_02140418);
}
#pragma pop

// @symbol Jump3D_InitPool
extern "C" int Jump3D_InitPool(char *base, int count)
{
    int i;
    int off;
    data_ov006_02140420 = base;
    data_ov006_02140418 = count;
    data_ov006_02140428 = count;
    i = 0;
    if (count > 0) {
        off = i;
        do {
            char *q;
            if (!((dMgJump3DMario_c *)(data_ov006_02140420 + off))->LoadResources()) return 0;
            ((dMgJump3DMario_c *)(data_ov006_02140420 + off))->HideAndIdle();
            ((dMgJump3DMario_c *)(data_ov006_02140420 + off))->UpdateModelMatrix();
            q = data_ov006_02140420 + off;
            if (i < 3) data_ov006_021403f4[i] = q;
            i++;
            off += 0xb8;
        } while (i < data_ov006_02140418);
    }
    data_ov006_0213b010 = -0x1e00;
    return 1;
}

// @symbol _ZN16dMgJump3DMario_c12Unk_020c762cEv
/* Vtable slot 2. */
int dMgJump3DMario_c::Unk_020c762c()
{
    int r = 0;
    int m = 1;

    if (mState == pmf_dMgJump3DMario_c_StateIdle_3.pmf)
        m = 0;
    if (m != 0) {
        m = 1;
        if (mState == pmf_dMgJump3DMario_c_StateWindUp_3.pmf)
            m = 0;
        if (m != 0)
            r = 1;
    }
    return r;
}


// @symbol _ZN16dMgJump3DMario_c12Unk_020c76d0Ev
/* Vtable slot 1. */
void *dMgJump3DMario_c::Unk_020c76d0()
{
    return &mVel;
}


// @symbol _ZN16dMgJump3DMario_c12Unk_020c76d8Ev
/* Vtable slot 0, and the key function. */
void *dMgJump3DMario_c::Unk_020c76d8()
{
    return &mPos;
}


// @symbol _ZN16dMgJump3DMario_c17UpdateModelMatrixEv
/* Rebuilds the model matrix from mPos and mAngleY. */
void dMgJump3DMario_c::UpdateModelMatrix()
{
    Matrix4x3_FromTranslation(&data_020a0e68, mPos.x, mPos.y, mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    mModelAnim.mat4x3 = data_020a0e68;
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
        Camera_ProjectToScreen16(data_ov006_02141a44, &mPos, screen);

        angle = data_ov006_02140404;
        wave = data_02082214[(angle >> 4) * 2];
        screen[1] = screen[1] - (((wave << 2) + 0x30000) >> 12);
        scaleX = func_02053200((wave >> 2) + 0x1000);

        angle = data_ov006_02140404;
        scaleY = -func_02053200((data_02082214[(angle >> 4) * 2 + 1] >> 2) + 0x1000);

        /* Zero it through `int *`: writing mtx.a..mtx.d instead costs 11 words
           (notes/experiments/jump3d-2711-mtx-intptr-zeroing.md). */
        int *mp = (int *)&mtx;
        mp[0] = 0; mp[1] = 0; mp[2] = 0; mp[3] = 0;
        mtx.d = scaleY;
        mtx.a = scaleX;
        OAM::Render(false, data_ov006_02134d1c, screen[0], screen[1], -1, -1, (Matrix2x2 *)&mtx);
    }

    mModelAnim.Render((const Vector3 *)data_ov006_0212ddd0);
}

// @symbol _ZN16dMgJump3DMario_c6UpdateEv
/* The per-frame update: gravity, integrate, dispatch mState. */
void dMgJump3DMario_c::Update()
{
    ApproachLinear(mVel.y, data_ov006_0213b010, data_ov006_0213b018);
    AddVec3(&mPos, &mVel, &mPos);
    (this->*mState)();
    Camera_ProjectToScreen16(data_ov006_02141a40, &mPos, &mScreenX);
    UpdateModelMatrix();
    mModelAnim.dExtFrameCtrl_c::Advance();
}

// @symbol _ZN16dMgJump3DMario_c9StateDampEv
void dMgJump3DMario_c::StateDamp()
{
    mTimer -= 1;
    if (mTimer == 0) {
        ApproachLinear(data_ov006_02140428, 0, 1);
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
    if (data_ov006_02140428 > 1)
        Sound::PlayBank2_2D(0x1ca);
    else
        Sound::PlayBank2_2D(0x1c9);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_0214042c, 0, kAnimSpeed, 0);
    mTimer = 0x28;
    mState = *(Jump3DState *)&pmf_dMgJump3DMario_c_StateDamp;
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
        mVel.y = data_ov006_0213b008;
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
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_02140408, 0x40000000, kAnimSpeed, 0);
    mModelAnim.dExtFrameCtrl_c::currFrame = 0;
    Sound_PlayBank1Panned(0, 6, mPos.x);
    mTimer = 0x20;
    mState = *(Jump3DState *)&pmf_dMgJump3DMario_c_StateHold;
}


// @symbol _ZN16dMgJump3DMario_c9StateMoveEv
void dMgJump3DMario_c::StateMove()
{
    u16 st = mCommand;

    if (st == 1) {
        mCommand = 0;
        mVel.y = data_ov006_0213b008;
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
                    int *p = &data_ov006_0213b00c;
                    int t0 = (ax - 0x80) << 12;
                    int t1 = (-az) << 12;
                    v[0] = t0;
                    v[1] = t1;
                    /* Keep the volatile store: a plain `v[2] = 0;` shrinks the
                       function to 0x3a8 bytes against the ROM's 0x3c4
                       (notes/experiments/jump3d-2711-statemove-volatile-store.md). */
                    *(volatile int *)&v[2] = 0;
                    mVel.y = *p;
                    mVel.x = data_ov006_0213b01c * dx;
                    if (mAnimIdx != 0)
                        mAnimIdx = 0;
                    else
                        mAnimIdx = 1;
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
                    mModelAnim.dExtFrameCtrl_c::currFrame = 0;
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xed, mPos.x << 3, mPos.y << 3, mPos.z << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xee, v[0] << 3, v[1] << 3, v[2] << 3);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xef, v[0] << 3, v[1] << 3, v[2] << 3);
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
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
                mModelAnim.dExtFrameCtrl_c::currFrame = 0;
                Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
            }
        } else if (pos > kWallX && mVel.x > 0) {
            mVel.x = -FX_MUL(mVel.x, 0xd00);
            if (mVel.y > 0) {
                mAnimIdx = 2;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
                mModelAnim.dExtFrameCtrl_c::currFrame = 0;
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
    mState = *(Jump3DState *)&pmf_dMgJump3DMario_c_StateMove;
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
    if (mState == pmf_dMgJump3DMario_c_StateRespawn_2.pmf) {
        mVel.y = 0;
        HideAndIdle();
    } else {
        mVel.x = 0;
        mVel.y = 0x2000;
        Sound::PlayBank2_2D(0x1c9);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_0214042c, 0, kAnimSpeed, 0);
        mModelAnim.dExtFrameCtrl_c::currFrame = 0;
        mState = pmf_dMgJump3DMario_c_StateFallOut.pmf;
    }
}

// @symbol _ZN16dMgJump3DMario_c12StateRiseOutEv
void dMgJump3DMario_c::StateRiseOut()
{
    if (mModelAnim.file == data_ov006_0214041c)
    {
        if (mModelAnim.dExtFrameCtrl_c::WillHitFrame(0xc) ||
            mModelAnim.dExtFrameCtrl_c::WillHitFrame(0x18))
            D3D_PlaySeAtX(0x1b5, mPos.x);
    }
    if (mPos.y <= 0x180000)
        return;
    mVel.y = 0;
    data_ov006_02140434++;
    HideAndIdle();
}


// @symbol _ZN16dMgJump3DMario_c12StartRiseOutEv
void dMgJump3DMario_c::StartRiseOut()
{
    mVel.x = 0;
    mVel.y = data_ov006_0213b00c;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_0214041c, 0x40000000, kAnimSpeed, 0);
    mModelAnim.dExtFrameCtrl_c::currFrame = 0;
    Sound::PlayBank2_2D(0x10f);
    func_02012718(0x1b5, mScreenX << 12);
    mState = *(Jump3DState *)&pmf_dMgJump3DMario_c_StateRiseOut;
}

// @symbol _ZN16dMgJump3DMario_c8EnterHitEv
void dMgJump3DMario_c::EnterHit()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_0214041c, 0x40000000, kAnimSpeed, 0);
    mModelAnim.dExtFrameCtrl_c::currFrame = 0;
    if (mState == pmf_dMgJump3DMario_c_StateWindUp_2.pmf) {
        func_02012718(0x110, mScreenX << 12);
    } else {
        Sound_PlayBank1Panned(0, 4, mPos.x);
    }
    func_02012718(0x1b5, mScreenX << 12);
    mState = pmf_dMgJump3DMario_c_StateBounce_2.pmf;
}


// @symbol _ZN16dMgJump3DMario_c11StateBounceEv
void dMgJump3DMario_c::StateBounce()
{
    if (mVel.y < 0) {
        EnterMove();
        StateMove();
        return;
    }

    if (mModelAnim.file == data_ov006_0214041c) {
        if (mModelAnim.dExtFrameCtrl_c::WillHitFrame(0xc) != 0 ||
            mModelAnim.dExtFrameCtrl_c::WillHitFrame(0x18) != 0)
            D3D_PlaySeAtX(0x1b5, mPos.x);
    }

    if (mPos.x < -kWallX && mVel.x < 0) {
        mVel.x = -FX_MUL(mVel.x, 0xd00);
        if (mVel.y > 0) {
            mAnimIdx = 2;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
            mModelAnim.dExtFrameCtrl_c::currFrame = 0;
            Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
        }
    } else if (mPos.x > kWallX && mVel.x > 0) {
        mVel.x = -FX_MUL(mVel.x, 0xd00);
        if (mVel.y > 0) {
            mAnimIdx = 2;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *data_ov006_0213b098[mAnimIdx], 0x40000000, kAnimSpeed, 0);
            mModelAnim.dExtFrameCtrl_c::currFrame = 0;
            Sound_PlayBank1Panned(0, mAnimIdx, mPos.x);
        }
    } else {
        u16 t = mCommand;
        if (t == 1) {
            mCommand = 0;
            mVel.y = data_ov006_0213b008;
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
    mState = *(Jump3DState *)&pmf_dMgJump3DMario_c_StateBounce;
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
    mVel.y = data_ov006_0213b00c;
    mCommand = 0;
    EnterHit();
}


// @symbol _ZN16dMgJump3DMario_c11StartWindUpEi
void dMgJump3DMario_c::StartWindUp(int timer)
{
    mTimer = (s16)timer;
    mState = *(Jump3DState *)&pmf_dMgJump3DMario_c_StateWindUp;
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
    mState = *(Jump3DState *)&pmf_dMgJump3DMario_c_StateIdle_2;
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
        speed = data_ov006_0213b01c;
        rnd = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        mVel.x = FX_MUL((((int)rnd - 0x800) << 1), speed);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_02140424, 0x40000000, kAnimSpeed, 0);
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
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov006_02140424, 0x40000000, kAnimSpeed, 0);
    mState = *(Jump3DState *)&pmf_dMgJump3DMario_c_StateRespawn;
}


// @symbol _ZN16dMgJump3DMario_c13LoadResourcesEv
/* Loads the model and the six anims. */
int dMgJump3DMario_c::LoadResources()
{
    /* Keep `t`: folding the comparison into the `if` below shrinks
       the function to 0x160 bytes against the ROM's 0x16c
       (notes/experiments/jump3d-2711-87d0-int-temp.md). */
    int t;

    if (func_020179b4(&data_ov006_02140450, &mModelAnim, 1) == 0)
        return 0;

    data_ov006_02140430 = (BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov006_02140460);
    data_ov006_0214040c = (BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov006_02140468);
    data_ov006_0214041c = (BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov006_02140458);
    data_ov006_02140424 = (BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov006_02140438);
    data_ov006_02140408 = (BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov006_02140440);
    data_ov006_0214042c = (BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov006_02140448);

    mAnimIdx = 0;
    if (data_ov006_02141a40 != 0)
        Camera_ProjectToScreen16(data_ov006_02141a40, &mPos, &mScreenX);

    t = data_0209f5c0->actorID == 0x175;
    if (t != 0) {
        func_02016a14(&mModelAnim, 0x7fff);
        func_02016a04(&mModelAnim, 0x210);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, data_ov006_02140430, 0x40000000, kAnimSpeed, 0);

    HideAndIdle();
    return 1;
}

// @symbol _ZN16dMgJump3DMario_cD1Ev
dMgJump3DMario_c::~dMgJump3DMario_c()
{
    data_ov006_02140450.Release();
    data_ov006_02140460.Release();
    data_ov006_02140468.Release();
    data_ov006_02140458.Release();
    data_ov006_02140438.Release();
    data_ov006_02140440.Release();
    data_ov006_02140448.Release();
    data_ov006_02140430 = 0;
    data_ov006_0214040c = 0;
    data_ov006_02140424 = 0;
    data_ov006_02140408 = 0;
    data_ov006_0214042c = 0;
}


// @symbol _ZN16dMgJump3DMario_cC1Ev
/* Empty: the base constructor, the vptr store and the ModelAnim member
 * construction are all generated. */
dMgJump3DMario_c::dMgJump3DMario_c()
{
}

extern "C" const int data_ov006_0212ddd0[3] = {0x2000, 0x2000, 0x2000};

extern "C" void _ZN16dMgJump3DMario_c9StateMoveEv(dMgJump3DMario_c *);
extern "C" void _ZN16dMgJump3DMario_c9StateHoldEv(dMgJump3DMario_c *);
extern "C" void _ZN16dMgJump3DMario_c9StateDampEv(dMgJump3DMario_c *);
extern "C" void _ZN16dMgJump3DMario_c12StateRespawnEv(dMgJump3DMario_c *);
extern "C" void _ZN16dMgJump3DMario_c9StateIdleEv(dMgJump3DMario_c *);
extern "C" void _ZN16dMgJump3DMario_c11StateWindUpEv(dMgJump3DMario_c *);
extern "C" void _ZN16dMgJump3DMario_c11StateBounceEv(dMgJump3DMario_c *);
extern "C" void _ZN16dMgJump3DMario_c12StateRiseOutEv(dMgJump3DMario_c *);
extern "C" void _ZN16dMgJump3DMario_c12StateFallOutEv(dMgJump3DMario_c *);
extern "C" unsigned pmf_dMgJump3DMario_c_StateMove[2] = { (unsigned)_ZN16dMgJump3DMario_c9StateMoveEv, 0 };
extern "C" unsigned pmf_dMgJump3DMario_c_StateHold[2] = { (unsigned)_ZN16dMgJump3DMario_c9StateHoldEv, 0 };
extern "C" unsigned pmf_dMgJump3DMario_c_StateDamp[2] = { (unsigned)_ZN16dMgJump3DMario_c9StateDampEv, 0 };
extern "C" unsigned pmf_dMgJump3DMario_c_StateRespawn[2] = { (unsigned)_ZN16dMgJump3DMario_c12StateRespawnEv, 0 };
extern "C" unsigned pmf_dMgJump3DMario_c_StateIdle[2] = { (unsigned)_ZN16dMgJump3DMario_c9StateIdleEv, 0 };
extern "C" unsigned pmf_dMgJump3DMario_c_StateIdle_2[2] = { (unsigned)_ZN16dMgJump3DMario_c9StateIdleEv, 0 };
extern "C" unsigned pmf_dMgJump3DMario_c_StateWindUp[2] = { (unsigned)_ZN16dMgJump3DMario_c11StateWindUpEv, 0 };
extern "C" Jump3DRec pmf_dMgJump3DMario_c_StateIdle_3 = {{ (unsigned)_ZN16dMgJump3DMario_c9StateIdleEv, 0 }};
extern "C" unsigned pmf_dMgJump3DMario_c_StateBounce[2] = { (unsigned)_ZN16dMgJump3DMario_c11StateBounceEv, 0 };
extern "C" Jump3DRec pmf_dMgJump3DMario_c_StateWindUp_2 = {{ (unsigned)_ZN16dMgJump3DMario_c11StateWindUpEv, 0 }};
extern "C" Jump3DRec pmf_dMgJump3DMario_c_StateWindUp_3 = {{ (unsigned)_ZN16dMgJump3DMario_c11StateWindUpEv, 0 }};
extern "C" Jump3DRec pmf_dMgJump3DMario_c_StateBounce_2 = {{ (unsigned)_ZN16dMgJump3DMario_c11StateBounceEv, 0 }};
extern "C" unsigned pmf_dMgJump3DMario_c_StateRiseOut[2] = { (unsigned)_ZN16dMgJump3DMario_c12StateRiseOutEv, 0 };
extern "C" Jump3DRec pmf_dMgJump3DMario_c_StateRespawn_2 = {{ (unsigned)_ZN16dMgJump3DMario_c12StateRespawnEv, 0 }};
extern "C" Jump3DRec pmf_dMgJump3DMario_c_StateFallOut = {{ (unsigned)_ZN16dMgJump3DMario_c12StateFallOutEv, 0 }};
extern "C" BCA_File **data_ov006_0213b098[3] = {
    &data_ov006_02140430,
    &data_ov006_0214040c,
    &data_ov006_02140424,
};

/* Zero image. These sit in front of the file-handle constructors, so the
 * .bss claim starts here and the constructors stay at 0x02140438. */
extern "C" {
BCA_File *data_ov006_02140408;
BCA_File *data_ov006_0214040c;
int data_ov006_02140410;
int data_ov006_02140414;
int data_ov006_02140418;
BCA_File *data_ov006_0214041c;
char *data_ov006_02140420;
BCA_File *data_ov006_02140424;
int data_ov006_02140428;
BCA_File *data_ov006_0214042c;
BCA_File *data_ov006_02140430;
int data_ov006_02140434;
}

Jump3DMarioAnimFilePtr data_ov006_02140460(32888);
Jump3DMarioAnimFilePtr data_ov006_02140468(32797);
Jump3DMarioAnimFilePtr data_ov006_02140458(32910);
Jump3DMarioAnimFilePtr data_ov006_02140438(32958);
Jump3DMarioAnimFilePtr data_ov006_02140440(32914);
Jump3DMarioAnimFilePtr data_ov006_02140448(32932);
Jump3DModelFilePtr data_ov006_02140450(534);
