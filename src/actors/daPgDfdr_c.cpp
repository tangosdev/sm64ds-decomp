//cpp
/* Penguin Defender (PENGUIN_DEFENDER 258) -- ov027/daPgDfdr_c.
 *
 * ov027 is mixed (sliding ice / chill bully / Bubba / snowman breath).
 * RTTI names this class daPgDfdr_c; the debug table names PENGUIN_DEFENDER.
 *
 * common.h FIRST: func_ov027_02111994 assigns a whole Matrix4x3, and the ROM
 * copies it as three 4-word ldm/stm pairs. common.h's flat s32 m[12] is that
 * copy; math/Matrix.h's nested {Matrix3x3 r; Vector3 t;} splits it (0x94 ->
 * 0xac).
 *
 * deslop leftovers:
 * - SetAnim / TextureSequence::SetFile / dBgW_KcMbg::SetFile / dCcAc_c::Init
 *   6az: this TU's InitResources and func_ov027_02111ca8 / 02111b2c pass
 *   Fix12<int> by value; the header method form size-DIFFs.
 * - dBgActor_c::IsClsnInRangeOnScreen 6az: Behavior calls it; the header
 *   method form is refused by the bytes (include/dBgActor_c.h).
 * - func_020393d4: InitResources stores dBgW::UpdatePosAndAngs on
 *   mMeshCollider; dBgW.h has no setter.
 * - func_0201267c: func_ov027_02111a28 plays 0xf3 at mCamSpacePosX on the
 *   walk animation's footstep frames.
 * - SharedFilePtr +4: InitResources' TextureSequence::Prepare / SetFile
 *   read the BMD/BTP at data_ov027_02113c7c / 02113c94 +4;
 *   SharedFilePtr.h has no fields.
 * - data_ov027_* model/KCL/BCA/CLPS handles and the four aliasing
 *   stride-0xc step symbols (02113a1c / 20 / 24 / 26); this TU consumes
 *   them, overlay .data owns them.
 * - S14: g_profile_PENGUIN_DEFENDER stays outside the licensed .text.
 * - func_ov027_02111a28 keeps the (int)c+0x3d9 / +0x3d4 pointer forms and
 *   the c+0x378 / +0x380 / +0x74 loads: named mStepIndex++ / mDistanceLeft
 *   -= / mModelAnim.currFrame / file / mCamSpacePosX CSE the field address.
 * - func_ov027_02111d38 / 02111cfc keep the incomplete C/PMF stand-in over
 *   mStateTable; completing to daPgDfdr_c::* is a pointer-to-member
 *   representation change.
 */

#include "common.h"
#include "daPgDfdr_c.h"
#include "decl_Player.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"
#include "dBgCh_Gnd.h"

struct BMD_File;
struct BTP_File;

typedef struct Elem {
    int a, b, c, d;
} Elem;

struct C;
typedef void (C::*PMF)();
struct C { char pad[0x3cc]; PMF *pp; };

extern "C" {
void Matrix4x3_FromRotationY(void *, short);
int DecIfAbove0_Byte(void *p);
int _Z14ApproachLinearRsss(short *r, short t, short s);
void func_0201267c(int a, char *b);
void func_020393d4(void *p, int v);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int b, unsigned int c);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *o, void *f, int i, int fx, unsigned j);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *o, void *kcl, void *m, int fx, short s, void *clps);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *o, void *act, int a, int b, unsigned c, unsigned d);
void _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *t, int a, int b);

void func_ov027_02111994(daPgDfdr_c *self);
int func_ov027_02111a28(char *c);
void func_ov027_02111b2c(daPgDfdr_c *self);
int func_ov027_02111c48(daPgDfdr_c *self);
int func_ov027_02111ca8(daPgDfdr_c *self);
void func_ov027_02111d38(void *cv);

extern char data_ov027_02113c6c;
extern char data_ov027_02113c7c;
extern char data_ov027_02113c94;
extern void *data_ov027_02113c84[];
extern int data_ov027_02113c74[];
extern int data_ov027_02113c8c[];
extern char data_ov027_02113a1c[];
extern char data_ov027_02113a20[];
extern char data_ov027_02113a24[];
extern char data_ov027_02113a26[];
extern Elem data_ov027_02113ce4[];
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daPgDfdr_c_classInit
extern "C" daPgDfdr_c *daPgDfdr_c_classInit()
{
    return new daPgDfdr_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c13InitResourcesEv
s32 daPgDfdr_c::InitResources()
{
    int i;
    void *f;
    Vector3 pos;

    f = Model::LoadFile(*(SharedFilePtr *)&data_ov027_02113c7c);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);

    for (i = 0; i < 3; i++)
        Animation::LoadFile(*(SharedFilePtr *)data_ov027_02112ca4[i]);

    TextureSequence::LoadFile(*(SharedFilePtr *)&data_ov027_02113c94);
    TextureSequence::Prepare(**(BMD_File **)(&data_ov027_02113c7c + 4),
                             **(BTP_File **)(&data_ov027_02113c94 + 4));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence, *(void **)(&data_ov027_02113c94 + 4), 0, 0x1000, 0);

    mAngleY = (short)0xdd30;
    mPrevAngleY = mAngleY;
    mPosX = 0x6c4000;
    mPosY = 0xcb2000;
    mPosZ = 0x182bb8;
    func_ov027_02111994(this);

    f = dBgW_Kc::LoadFile(*(SharedFilePtr *)&data_ov027_02113c6c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, f, &mClsnMat, 0x199, mAngleY, &data_ov027_021130e8);
    func_020393d4(&mMeshCollider, (int)&dBgW::UpdatePosAndAngs);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x82000, 0xc8000, 0x800004, 0);
    func_ov027_02111d70(this, 1);

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

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c8BehaviorEv
s32 daPgDfdr_c::Behavior()
{
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    func_ov027_02111cfc(this);
    if (_ZN6Player16IsInsideOfCannonEv(ClosestPlayer())) {
        mFlags &= ~2;
    } else {
        mFlags |= 2;
    }
    static_cast<Animation *>(&mModelAnim)->Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    func_ov027_02111994(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c6RenderEv
s32 daPgDfdr_c::Render()
{
    mTextureSequence.Update(mModelAnim.data);
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c16OnPendingDestroyEv
void daPgDfdr_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daPgDfdr_c16CleanupResourcesEv
s32 daPgDfdr_c::CleanupResources()
{
    int i;
    (*(SharedFilePtr *)&data_ov027_02113c7c).Release();
    for (i = 0; i < 3; i++) {
        ((SharedFilePtr *)data_ov027_02112ca4[i])->Release();
    }
    (*(SharedFilePtr *)&data_ov027_02113c94).Release();
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    (*(SharedFilePtr *)&data_ov027_02113c6c).Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111d70
extern "C" void func_ov027_02111d70(void *self, int idx)
{
    daPgDfdr_c *c = (daPgDfdr_c *)self;
    c->mStateTable = &data_ov027_02113ce4[idx];
    func_ov027_02111d38(self);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111d38
extern "C" void func_ov027_02111d38(void *cv)
{
    C *c = (C *)cv;
    PMF *p = c->pp;
    (c->**p)();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111cfc
extern "C" void func_ov027_02111cfc(void *cv)
{
    C *c = (C *)cv;
    PMF *p = c->pp + 1;
    (c->**p)();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111ca8
extern "C" int func_ov027_02111ca8(daPgDfdr_c *self)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, data_ov027_02113c84[1], 0, 0x1000, 0);
    self->mModelAnim.speed = 0x1000;
    self->mTimer = 0x14;
    self->unk_3d0 = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111c48
extern "C" int func_ov027_02111c48(daPgDfdr_c *self)
{
    if (DecIfAbove0_Byte(&self->mTimer) == 0) {
        unsigned char idx = self->mStepIndex;
        short val = *(short *)(data_ov027_02113a26 + idx * 0xc);
        if (_Z14ApproachLinearRsss(&self->mAngleY, val, 0x514) != 0) {
            func_ov027_02111d70(self, 1);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111b2c
extern "C" void func_ov027_02111b2c(daPgDfdr_c *self)
{
    self->mDistanceLeft = *(int *)(data_ov027_02113a1c + self->mStepIndex * 0xc);
    self->mHorzSpeed = *(int *)(data_ov027_02113a20 + self->mStepIndex * 0xc);
    self->mPrevAngleY = *(short *)(data_ov027_02113a24 + self->mStepIndex * 0xc);
    if (*(short *)(data_ov027_02113a24 + self->mStepIndex * 0xc) != *(short *)(data_ov027_02113a26 + self->mStepIndex * 0xc)) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov027_02113c8c[1], 0, 0x1000, 0);
        self->mModelAnim.speed = 0x1000;
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&self->mModelAnim, (void *)data_ov027_02113c74[1], 0, 0x1000, 0);
        self->mModelAnim.speed = (int)(((s64) *(int *)(data_ov027_02113a20 + self->mStepIndex * 0xc) * 0x5000 + 0x800) >> 12);
    }
    self->unk_3d0 = 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111a28
extern "C" int func_ov027_02111a28(char *c)
{
    daPgDfdr_c *self = (daPgDfdr_c *)c;
    int d = self->mDistanceLeft;
    if (d == 0) {
        unsigned char *p = (unsigned char *)(((int)c + 0x3d9));
        *p = *p + 1;
        if (self->mStepIndex >= 9) self->mStepIndex = 0;
        func_ov027_02111d70(c, 0);
        return 1;
    }
    {
        int v = self->mHorzSpeed;
        if (d < v) {
            self->mHorzSpeed = d;
            self->mDistanceLeft = 0;
        } else {
            int *q = (int *)(((int)c + 0x3d4));
            *q = *q - v;
        }
    }
    self->UpdatePos(&self->mdCcAc_c);
    {
        int t = *(int *)(c + 0x378);
        int w = data_ov027_02113c74[1];
        int field = w ? *(int *)(c + 0x380) : *(int *)(c + 0x380);
        unsigned int id = (unsigned int)(t << 4) >> 0x10;
        if (field == w) {
            if (id == 0xa || id == 0x16) {
                func_0201267c(0xf3, c + 0x74);
            }
        } else if (field == data_ov027_02113c8c[1]) {
            if (id == 9 || id == 0x16) {
                func_0201267c(0xf3, c + 0x74);
            }
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov027_02111994
extern "C" void func_ov027_02111994(daPgDfdr_c *self)
{
    Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
    self->mModelAnim.mat4x3.m[9] = self->mPosX >> 3;
    self->mModelAnim.mat4x3.m[10] = self->mPosY >> 3;
    self->mModelAnim.mat4x3.m[11] = self->mPosZ >> 3;
    self->mClsnMat = self->mModelAnim.mat4x3;
    self->mClsnMat.m[9] = self->mPosX;
    self->mClsnMat.m[10] = self->mPosY;
    self->mClsnMat.m[11] = self->mPosZ;
    self->mMeshCollider.Transform(self->mClsnMat, self->mAngleY);
}
