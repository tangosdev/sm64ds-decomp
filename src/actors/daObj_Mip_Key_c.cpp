//cpp
/* The key dropped by Mip chases the player, then runs the save conversation.
 * The loose/carried states each have an entry and an update callback.
 *
 * Four callbacks are stored as PMFs in the ROM. Their address-derived entry
 * names remain pending reconstruction; the caller census does not establish
 * original file-static linkage. See notes/experiments/mip-key-2853-source-forms.md
 * for the boundary evidence and retained ABI bridges.
 *
 * Leftover: unk_0a4/0ac/190/19c are unrecovered header fields (0a4/0ac
 *   ride the loose-update approach; 19c selects the entry).
 * Leftover: data_020a0e68 is shared arm9 matrix scratch.
 * Leftover: func_02012790 / func_02013868 are arm9 stubs.
 * Leftover: the file home is sinit-constructed
 *   (src/__sinit_ov085_0212f5ec.c); g_profile stays where the
 *   registry owns it (S14).
 */

#include "daObj_Mip_Key_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Camera.h"
#include "Message.h"

extern "C" {

void func_ov085_0212cd0c(daObj_Mip_Key_c *self);
int func_ov085_0212d268(daObj_Mip_Key_c *c, MipKeyState *p);
void func_ov085_0212d2b8(daObj_Mip_Key_c *thiz);

s16   Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
s16   Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
void  Vec3_Asr(Vector3 *dst, Vector3 *src, int sh);
u16   DecIfAbove0_Short(u16 *p);
void  Matrix4x3_FromRotationY(void *m, int angY);
void  Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 angX);
void  Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void  MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);

// Scalar bridge: the real Fix12 aggregate call grew this helper by 16 bytes.
void  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
          dActor_c *thiz, ShadowModel *sm, Matrix4x3 *mtx, int rad, int height, u8 flags);

bool  _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, Fix12i d, int loop);

unsigned int func_02012790(unsigned int a);
void  func_02013868(int t, int x);
void  StartMinigameMenu(u8 a);

extern Camera *data_0209f318;
extern int   data_0209caa0[];
extern u8    data_0209d660;
extern u8    data_0209d684;
extern Matrix4x3 data_020a0e68;
extern SharedFilePtr data_ov085_021305d8;
extern MipKeyState data_ov085_0213071c;
extern MipKeyState data_ov085_0213072c;

}

int ApproachLinear(int &cur, int tgt, int step);

#pragma defer_codegen off

// @symbol _ZN15daObj_Mip_Key_cD1Ev
// @symbol _ZN15daObj_Mip_Key_cD0Ev

daObj_Mip_Key_c::~daObj_Mip_Key_c()
{
}

// End the conversation, restore control flags and retire the key.
// @symbol func_ov085_0212cd0c

extern "C" void func_ov085_0212cd0c(daObj_Mip_Key_c *c)
{
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x7f, 0, 0x7444, 0);
    Message::EndTalk();
    data_0209f318->mFlags &= ~8;
    c->mPlayer->mStateFlags &= ~0x800;
    c->MarkForDestruction();
}

// Carried update: follow the player and advance the save conversation.
// @symbol func_ov085_0212cd80

extern "C" int func_ov085_0212cd80(daObj_Mip_Key_c *self)
{
    Player* other = self->mPlayer;
    unsigned char mode;
    int* playerPos;

    if (other == 0) return 1;

    playerPos = (int*)(&other->mPosX);
    Vector3 v;
    v = *(Vector3*)playerPos;
    v.y += 0xc8000;
    ApproachLinear(self->mPosY, v.y, 0xa000);

    self->mPosX = v.x;
    self->mPosZ = v.z;
    mode = data_0209d684;

    switch (self->mTalkState) {
    case 0:
        if (!_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x12, 0x7f, 0x15ccc, 0))
            break;
        self->mTalkState = 1;
    case 1:
        if (data_0209d660 == 0) {
            if (mode == 1) {
                func_02012790(0x5e);
                Message::DisplaySaving(0x295);
                self->mTalkState = 2;
            } else if (mode == 2) {
                func_02012790(0x5e);
                Message::DisplaySaving(0x295);
                self->mTalkState = 3;
            } else if (mode == 3) {
                func_02012790(0x98);
                if (self->mWaitForTalk) {
                    self->mTalkState = 0xa;
                } else {
                    func_ov085_0212cd0c(self);
                }
            }
        }
        break;
    case 2:
        if (data_0209d660 == 0) {
            if (self->mWaitForTalk) {
                self->mTalkState = 0xa;
            } else {
                func_ov085_0212cd0c(self);
            }
        }
        break;
    case 3:
        if (data_0209d660 == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x7f, 0, 0x7444, 0);
            StartMinigameMenu(1);
            Message::EndTalk();
            self->MarkForDestruction();
        }
        break;
    case 4:
        {
            int shown;
            shown = self->mPlayer->ShowMessage(*self, 0x18b, 0, 1, 0);
            if (shown == 1) {
                self->mWaitForTalk = 1;
                self->mTalkState = 0;
            }
        }
        break;
    case 6:
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x12, 0x7f, 0x15ccc, 0)) {
            if (data_0209d660 == 0) {
                if (mode == 1) {
                    func_02012790(0x5e);
                    Message::DisplaySaving(0x295);
                    self->mTalkState = 2;
                } else if (mode == 2) {
                    func_02012790(0x98);
                    func_ov085_0212cd0c(self);
                }
            }
        }
        break;
    case 10:
        if (self->mPlayer->HasFinishedTalking() == 1) {
            func_ov085_0212cd0c(self);
        }
        break;
    }
    return 1;
}

// Carried entry: stop moving and select the conversation.
// @symbol func_ov085_0212d038

extern "C" int func_ov085_0212d038(daObj_Mip_Key_c *c)
{
    int kind;
    c->unk_0a4 = 0;
    c->mVertSpeed = 0;
    c->unk_0ac = 0;
    c->mAngleX = 0;
    kind = c->unk_19c;
    if (kind != 0x4d) {
        if (kind != 7) {
            func_02013868(kind, c->mPlayer->param1);
            c->mPlayer->SetNoControlState(2, 0x189, 1);
            c->mTalkState = 0;
        } else {
            data_0209caa0[1] |= 0x40;
            c->mPlayer->SetNoControlState(2, 0x18c, 1);
            c->mTalkState = 6;
        }
    } else {
        data_0209caa0[1] |= 0x80;
        c->mPlayer->SetNoControlState(2, 0x149, 1);
        c->mTalkState = 6;
    }
    return 1;
}

// Loose update: turn toward the player and approach until caught.
// @symbol func_ov085_0212d108

extern "C" int func_ov085_0212d108(daObj_Mip_Key_c *c)
{
    Vector3 v;
    Vector3 out;
    Vector3 pp;
    Player* player;
    int zero;
    Vector3* ppos;

    c->mAngleX += 0x1000;
    if ((u16)c->mStateTimer != 0) {
        return 1;
    }
    player = c->ClosestPlayer();
    if (player == 0) {
        return 1;
    }

    zero = 0;
    ppos = (Vector3*)(&player->mPosX);
    v.x = zero;
    v.y = zero;
    v.z = zero;
    out.x = zero;
    out.y = zero;
    out.z = zero;
    pp.x = ppos->x;
    pp.y = ppos->y;
    pp.z = ppos->z;
    c->mPrevAngleY = Vec3_HorzAngle((Vector3*)&c->mPosX, &pp);
    c->mPrevAngleX = Vec3_VertAngle((Vector3*)&c->mPosX, &pp);
    v.z = 0x14000;
    Matrix4x3_FromRotationY(&data_020a0e68, c->mPrevAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, c->mPrevAngleX);
    MulVec3Mat4x3(&v, &data_020a0e68, &out);
    ApproachLinear(c->unk_0a4, out.x, 0x1000);
    ApproachLinear(c->mVertSpeed, out.y, 0x1000);
    ApproachLinear(c->unk_0ac, out.z, 0x1000);
    if (pp.y + 0x64000 > c->mPosY) {
        c->mPlayer = player;
        func_ov085_0212d268(c, &data_ov085_0213072c);
    }
    return 1;
}

// Loose entry: wait ten frames before chasing.
// @symbol func_ov085_0212d24c

extern "C" int func_ov085_0212d24c(daObj_Mip_Key_c *p)
{
    p->mStateTimer = 10;
    p->mVertSpeed = 81920;
    return 1;
}

// Install a state and run its entry callback.
// @symbol func_ov085_0212d268

extern "C" int func_ov085_0212d268(daObj_Mip_Key_c *c, MipKeyState *p)
{
    c->mState = p;
    if (c->mState->enter == 0)
        return 1;
    return (c->*c->mState->enter)();
}

// Build the render matrix and place the cylinder shadow.
// @symbol func_ov085_0212d2b8

extern "C" void func_ov085_0212d2b8(daObj_Mip_Key_c *thiz)
{
    // Whole Matrix4x3 assignment emits six extra words under 2004/b56.
    // Preserve the measured flat 48-byte copy until the shared type is resolved.
    typedef struct { int w[12]; } MipKeyM48;
    Vector3 v;
    Vec3_Asr(&v, (Vector3*)&thiz->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        thiz->mAngleX, thiz->mAngleY, thiz->mAngleZ);
    *(MipKeyM48*)(&thiz->mModel.mat4x3) = *(MipKeyM48*)&data_020a0e68;
    Matrix4x3_FromTranslation(&data_020a0e68,
        thiz->mPosX >> 3,
        (thiz->mPosY - 0x32000) >> 3,
        thiz->mPosZ >> 3);
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        thiz, &thiz->mShadowModel, &thiz->mModel.mat4x3, 0x23000, 0x12c000, 0xf);
}

// @symbol _ZN15daObj_Mip_Key_c16CleanupResourcesEv

int daObj_Mip_Key_c::CleanupResources()
{
    data_ov085_021305d8.Release();
    return 1;
}

// @symbol _ZN15daObj_Mip_Key_c16OnPendingDestroyEv

void daObj_Mip_Key_c::OnPendingDestroy()
{
}

// @symbol _ZN15daObj_Mip_Key_c6RenderEv

int daObj_Mip_Key_c::Render()
{
    if (mState == &data_ov085_0213072c) {
        mAngleY += 0x500;
    }
    func_ov085_0212d2b8(this);
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObj_Mip_Key_c8BehaviorEv

int daObj_Mip_Key_c::Behavior()
{
    DecIfAbove0_Short((u16*)&mStateTimer);
    if (mState->execute)
        (this->*mState->execute)();
    int nextSpeed = mVertSpeed + mVertAccel;
    int capped = mTerminalVelocity;
    if (nextSpeed >= capped)
        capped = nextSpeed;
    /* The read-back-and-store of unk_0ac is what the ROM does; it is not dead. */
    int unk0acCopy = unk_0ac;
    mVertSpeed = capped;
    unk_0ac = unk0acCopy;
    UpdatePosWithOnlySpeed(0);
    return 1;
}

// @symbol _ZN15daObj_Mip_Key_c13InitResourcesEv

int daObj_Mip_Key_c::InitResources()
{
    void* f = Model::LoadFile(data_ov085_021305d8);
    if (mModel.SetFile((BMD_File*)f, 1, -1) == 0)
        return 0;
    mShadowModel.InitCylinder();
    unk_19c = param1 & 0xff;
    mTerminalVelocity = -0x3c000;
    unk_190 = 0;
    func_ov085_0212d268(this, &data_ov085_0213071c);
    return 1;
}

// classInit is a lineage-supported project spelling; ROM RTTI proves the class.
// The exact original factory spelling is unknown (old alias: RabbitKey_Spawn).
// @symbol daObj_Mip_Key_c_classInit

extern "C" daObj_Mip_Key_c *daObj_Mip_Key_c_classInit(void)
{
    return new daObj_Mip_Key_c();
}
