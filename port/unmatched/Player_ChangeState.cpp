// HOST COPY of src/_ZN6Player11ChangeStateERNS_5StateE.cpp -- the two
// pointer-to-member dispatch sites replaced with hal_call_state_fn, which
// translates the DS code addresses baked in the overlay's State objects to
// host method calls (table: port/hal/player_states.inc). Everything else is
// the matched source verbatim. The State fields are read at raw DS offsets
// (fn word at +0, exit fn word at +0x10) because the objects are emitted
// overlay bytes, not host PMFs.
#include "decl_common.h"
#include "Player.h"

extern "C" {
extern void func_ov002_020d4540(char *p);
extern void func_ov002_020c9e18(char *c);
extern void func_0200d81c(void *thiz, int playerID);
extern int hal_call_state_fn(void *self, unsigned ds_addr);

extern void *data_0209f318;
extern unsigned char data_ov002_0211022c[];
extern unsigned char data_ov002_0211013c[];
extern unsigned char data_ov002_0211067c[];
extern unsigned char data_ov002_021106ac[];
extern unsigned char data_ov002_02110364[];
}

typedef unsigned char StateBytes;

// PORT_HOST_ABI: mwcc pointer-to-member dispatch; the State objects are
//   emitted overlay bytes read at raw DS offsets, not host PMFs. See the
//   header.
extern "C" int _ZN6Player11ChangeStateERNS_5StateE(struct Player *self,
                                                   StateBytes *newState) {
    *(StateBytes **)((char *)&self->mRequestedState) = newState;

    {
        StateBytes *cur;
        int ok;
        cur = *(StateBytes **)((char *)&self->mState);
        if (cur == 0) { ok = 1; goto check_ok; }
        if (*(int *)((char *)cur + 0x10) == 0) { ok = 1; goto check_ok; }
        ok = hal_call_state_fn(self, *(unsigned *)((char *)cur + 0x10));
    check_ok:
        if (!ok) return 0;
    }

    if (*(StateBytes **)((char *)&self->mState) == data_ov002_0211022c
        && (unsigned short)(self->mStateFlags & 0x400) == 0
        && self->mIsNoControl != 0
        && newState != data_ov002_0211013c
        && newState != data_ov002_0211067c
        && newState != data_ov002_021106ac
        && newState != data_ov002_0211022c)
    {
        return 0;
    }

    {
        int b = (self->mHeldObj != 0);
        if (b) {
        if (newState == data_ov002_0211013c) {
            self->mStateStep = 1;
            newState = data_ov002_02110364;
        }
        }
    }

    self->mLoopingSoundHandle = 0;
    self->mParticle3 = 0;
    self->mParticle2 = self->mParticle3;
    self->mParticle1 = self->mParticle2;

    *(StateBytes **)((char *)&self->mPrevState) =
        *(StateBytes **)((char *)&self->mState);
    *(StateBytes **)((char *)&self->mState) = newState;

    self->unk_717 = 0;
    self->mIsBodyClsnEnabled = 1;
    self->unk_716 = 0;

    self->unk_694 = 0;
    self->unk_690 = self->unk_694;

    self->mIsInAirState = 0;
    self->unk_654 = 0;

    self->mAngleZ = 0;
    self->mAngleX = self->mAngleZ;

    self->mIsControlDisabled = 0;
    self->unk_6ec = 0;
    self->unk_726 = 0;

    *(unsigned short *)(((long long)(int)((char *)&self->mStateFlags))) &= ~0x200;

    {
        int v1 = 0x4000;
        int v2 = 0x4b000;
        v1 = -v1;
        v2 = -v2;
        self->mVertAccel = v1;
        self->mTerminalVelocity = v2;
    }

    self->mStateTimer = 0;

    func_ov002_020d4540(((char *)self));

    self->mIsTakingDamage = 0;
    func_ov002_020c9e18(((char *)self));

    func_ov002_020e6780(((char *)self));

    if (data_0209f318 != 0) {
        func_0200d81c(data_0209f318, self->mPlayerNo);
    }

    {
        StateBytes *s = *(StateBytes **)((char *)&self->mState);
        if (*(int *)s == 0) return 1;
        return hal_call_state_fn(self, *(unsigned *)s);
    }
}
