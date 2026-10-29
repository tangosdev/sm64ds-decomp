//cpp
/* The key Mip drops. It chases the player, then runs the save conversation.
 * The loose and carried states each have an entry and an update.
 */
// Notes:
// - unk_190 and unk_19c stay the header's names. The loose update
//   approaches through mSpeedX/mSpeedZ, and 19c selects the entry.
// - func_02013868, StartMinigameMenu, Vec3_VertAngle, Vec3_Asr and
//   MulVec3Mat4x3 are arm9 C helpers no header declares yet.
// - The model handle data_ov085_021305d8 is constructed by daMip_c.
//   g_profile stays with the registry.

#include "daObj_Mip_Key_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "dCamera_c.h"
#include "Message.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

extern "C" {
s16 Vec3_VertAngle(const Vector3 *a, const Vector3 *b);
void Vec3_Asr(Vector3 *dst, Vector3 *src, int sh);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void func_02013868(int t, int x);
void StartMinigameMenu(u8 a);
}

extern dCamera_c *data_0209f318;
extern int data_0209caa0[];
extern u8 data_0209d660;
extern u8 data_0209d684;
/* MIPS's model-file handle, owned and constructed by d_a_mip.cpp's static
 * init. Same TU-local wrapper spelling as the owning TU. */
struct DaMipModelFile : SharedFilePtr {
    DaMipModelFile(u32 fileID);
    ~DaMipModelFile();
};
extern DaMipModelFile data_ov085_021305d8;
extern MipKeyState data_ov085_0213071c;
extern MipKeyState data_ov085_0213072c;

/* PMF literals in unowned .data; the initializer copies them pairwise
 * into the two state records below. */
extern MipKeyPMF data_ov085_02130184;
extern MipKeyPMF data_ov085_0213018c;
extern MipKeyPMF data_ov085_0213017c;
extern MipKeyPMF data_ov085_02130174;

#pragma defer_codegen off

// @symbol _ZN15daObj_Mip_Key_cD1Ev
// @symbol _ZN15daObj_Mip_Key_cD0Ev

daObj_Mip_Key_c::~daObj_Mip_Key_c() {}

// End the conversation, restore control flags and retire the key.
// @symbol _ZN15daObj_Mip_Key_c18FinishConversationEv

void daObj_Mip_Key_c::FinishConversation() {
    Sound::PlaySub(0x28, 0x7f, 0, 0x7444, 0);
    Message::EndTalk();
    data_0209f318->mFlags &= ~8;
    mPlayer->mStateFlags &= ~0x800;
    MarkForDestruction();
}

// Carried update: follow the player and advance the save conversation.
// @symbol _ZN15daObj_Mip_Key_c12StateCarriedEv

int daObj_Mip_Key_c::StateCarried() {
    daObj_Mip_Key_c *self = this;
    Player *other = self->mPlayer;
    unsigned char mode;
    int *playerPos;

    if (other == 0)
        return 1;

    playerPos = (int *)(&other->mPosX);
    Vector3 v;
    v = *(Vector3 *)playerPos;
    v.y += 0xc8000;
    ApproachLinear(self->mPosY, v.y, 0xa000);

    self->mPosX = v.x;
    self->mPosZ = v.z;
    mode = data_0209d684;

    switch (self->mTalkState) {
    case 0:
        if (!Sound::PlaySub(0x28, 0x12, 0x7f, 0x15ccc, 0))
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
                    self->FinishConversation();
                }
            }
        }
        break;
    case 2:
        if (data_0209d660 == 0) {
            if (self->mWaitForTalk) {
                self->mTalkState = 0xa;
            } else {
                self->FinishConversation();
            }
        }
        break;
    case 3:
        if (data_0209d660 == 0) {
            Sound::PlaySub(0x28, 0x7f, 0, 0x7444, 0);
            StartMinigameMenu(1);
            Message::EndTalk();
            self->MarkForDestruction();
        }
        break;
    case 4: {
        int shown;
        shown = self->mPlayer->ShowMessage(*self, 0x18b, 0, 1, 0);
        if (shown == 1) {
            self->mWaitForTalk = 1;
            self->mTalkState = 0;
        }
    } break;
    case 6:
        if (Sound::PlaySub(0x28, 0x12, 0x7f, 0x15ccc, 0)) {
            if (data_0209d660 == 0) {
                if (mode == 1) {
                    func_02012790(0x5e);
                    Message::DisplaySaving(0x295);
                    self->mTalkState = 2;
                } else if (mode == 2) {
                    func_02012790(0x98);
                    self->FinishConversation();
                }
            }
        }
        break;
    case 10:
        if (self->mPlayer->HasFinishedTalking() == 1) {
            self->FinishConversation();
        }
        break;
    }
    return 1;
}

// Carried entry: stop moving and select the conversation.
// @symbol _ZN15daObj_Mip_Key_c17StateCarriedEnterEv

int daObj_Mip_Key_c::StateCarriedEnter() {
    daObj_Mip_Key_c *c = this;
    int kind;
    c->mSpeedX = 0;
    c->mVertSpeed = 0;
    c->mSpeedZ = 0;
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
// @symbol _ZN15daObj_Mip_Key_c10StateLooseEv

int daObj_Mip_Key_c::StateLoose() {
    daObj_Mip_Key_c *c = this;
    Vector3 v;
    Vector3 out;
    Vector3 pp;
    Player *player;
    int zero;
    Vector3 *ppos;

    c->mAngleX += 0x1000;
    if ((u16)c->mStateTimer != 0) {
        return 1;
    }
    player = c->ClosestPlayer();
    if (player == 0) {
        return 1;
    }

    zero = 0;
    ppos = &player->mPos;
    v.x = zero;
    v.y = zero;
    v.z = zero;
    out.x = zero;
    out.y = zero;
    out.z = zero;
    pp.x = ppos->x;
    pp.y = ppos->y;
    pp.z = ppos->z;
    c->mPrevAngleY = Vec3_HorzAngle(&c->mPos, &pp);
    c->mPrevAngleX = Vec3_VertAngle(&c->mPos, &pp);
    v.z = 0x14000;
    Matrix4x3_FromRotationY(&data_020a0e68, c->mPrevAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, c->mPrevAngleX);
    MulVec3Mat4x3(&v, &data_020a0e68, &out);
    ApproachLinear(c->mSpeedX, out.x, 0x1000);
    ApproachLinear(c->mVertSpeed, out.y, 0x1000);
    ApproachLinear(c->mSpeedZ, out.z, 0x1000);
    if (pp.y + 0x64000 > c->mPosY) {
        c->mPlayer = player;
        c->ChangeState(&data_ov085_0213072c);
    }
    return 1;
}

// Loose entry: wait ten frames before chasing.
// @symbol _ZN15daObj_Mip_Key_c15StateLooseEnterEv

int daObj_Mip_Key_c::StateLooseEnter() {
    mStateTimer = 10;
    mVertSpeed = 81920;
    return 1;
}

// Install a state and run its entry callback.
// @symbol _ZN15daObj_Mip_Key_c11ChangeStateEP11MipKeyState

int daObj_Mip_Key_c::ChangeState(MipKeyState *p) {
    mState = p;
    if (mState->enter == 0)
        return 1;
    return (this->*mState->enter)();
}

// Build the render matrix and place the cylinder shadow.
// @symbol _ZN15daObj_Mip_Key_c20UpdateModelAndShadowEv

void daObj_Mip_Key_c::UpdateModelAndShadow() {
    daObj_Mip_Key_c *thiz = this;
    // Whole Matrix4x3 assignment emits six extra words under 2004/b56.
    // Preserve the measured flat 48-byte copy until the shared type is resolved.
    typedef struct {
        int w[12];
    } MipKeyM48;
    Vector3 v;
    Vec3_Asr(&v, &thiz->mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, thiz->mAngleX, thiz->mAngleY,
                                           thiz->mAngleZ);
    *(MipKeyM48 *)(&thiz->mModel.mat4x3) = *(MipKeyM48 *)&data_020a0e68;
    Matrix4x3_FromTranslation(&data_020a0e68, thiz->mPosX >> 3, (thiz->mPosY - 0x32000) >> 3,
                              thiz->mPosZ >> 3);
    thiz->DropShadowRadHeight(thiz->mShadowModel, thiz->mModel.mat4x3, 0x23000, 0x12c000, 0xf);
}

// @symbol _ZN15daObj_Mip_Key_c16CleanupResourcesEv

int daObj_Mip_Key_c::CleanupResources() {
    data_ov085_021305d8.Release();
    return 1;
}

// @symbol _ZN15daObj_Mip_Key_c16OnPendingDestroyEv

void daObj_Mip_Key_c::OnPendingDestroy() {}

// @symbol _ZN15daObj_Mip_Key_c6RenderEv

int daObj_Mip_Key_c::Render() {
    if (mState == &data_ov085_0213072c) {
        mAngleY += 0x500;
    }
    UpdateModelAndShadow();
    mModel.Render(0);
    return 1;
}

// @symbol _ZN15daObj_Mip_Key_c8BehaviorEv

int daObj_Mip_Key_c::Behavior() {
    DecIfAbove0_Short((u16 *)&mStateTimer);
    if (mState->execute)
        (this->*mState->execute)();
    int nextSpeed = mVertSpeed + mVertAccel;
    int capped = mTerminalVelocity;
    if (nextSpeed >= capped)
        capped = nextSpeed;
    /* The read-back-and-store of mSpeedZ is what the ROM does; it is not dead. */
    int unk0acCopy = mSpeedZ;
    mVertSpeed = capped;
    mSpeedZ = unk0acCopy;
    UpdatePosWithOnlySpeed(0);
    return 1;
}

// @symbol _ZN15daObj_Mip_Key_c13InitResourcesEv

int daObj_Mip_Key_c::InitResources() {
    void *f = dExtSimpleModel_c::LoadFile(data_ov085_021305d8);
    if (mModel.SetFile((BMD_File *)f, 1, -1) == 0)
        return 0;
    mShadowModel.InitCylinder();
    unk_19c = param1 & 0xff;
    mTerminalVelocity = -0x3c000;
    unk_190 = 0;
    ChangeState(&data_ov085_0213071c);
    return 1;
}

// classInit is a lineage-supported project spelling; ROM RTTI proves the class.
// The exact original factory spelling is unknown (old alias: RabbitKey_Spawn).
// @symbol daObj_Mip_Key_c_classInit

extern "C" daObj_Mip_Key_c *daObj_Mip_Key_c_classInit(void) {
    return new daObj_Mip_Key_c();
}

/* Static-init globals (was the handwritten __sinit_ov085_0212f9bc shard).
 * Each 16-byte state record takes its enter/execute PMFs from the .data
 * literals above. */
// @symbol __sinit_d_a_obj_mip_key.cpp
MipKeyState data_ov085_0213071c = {data_ov085_02130184, data_ov085_0213018c};
MipKeyState data_ov085_0213072c = {data_ov085_0213017c, data_ov085_02130174};
