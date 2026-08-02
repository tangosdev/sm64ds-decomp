//cpp
// @symbol _ZN6Player11ChangeStateERNS_5StateE
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Player.h"
struct C3;
typedef int (C3::*PMF)();

struct State {
    PMF onEnter;
    char pad[8];
    PMF onExit;
};

extern "C" {
extern void func_ov002_020d4540(char *p);
extern void func_ov002_020c9e18(char *c);
extern void func_0200d81c(void *thiz, int playerID);

extern void *data_0209f318;
extern State data_ov002_0211022c;
extern State data_ov002_0211013c;
extern State data_ov002_0211067c;
extern State data_ov002_021106ac;
extern State data_ov002_02110364;
}

extern "C" int _ZN6Player11ChangeStateERNS_5StateE(struct Player *self, State *newState) {
    *(State**)((char *)&self->unk_378) = newState;

    {
        State *cur;
        int ok;
        cur = *(State**)((char *)&self->unk_370);
        if (cur == 0) { ok = 1; goto check_ok; }
        if (*(int*)((char*)cur+0x10) == 0) { ok = 1; goto check_ok; }
        ok = (((C3*)((char *)self))->*(cur->onExit))();
    check_ok:
        if (!ok) return 0;
    }

    if (*(State**)((char *)&self->unk_370) == &data_ov002_0211022c
        && (unsigned short)(self->mStateFlags & 0x400) == 0
        && self->mIsNoControl != 0
        && newState != &data_ov002_0211013c
        && newState != &data_ov002_0211067c
        && newState != &data_ov002_021106ac
        && newState != &data_ov002_0211022c)
    {
        return 0;
    }

    {
        int b = (self->mHeldObj != 0);
        if (b) {
        if (newState == &data_ov002_0211013c) {
            self->mStateStep = 1;
            newState = &data_ov002_02110364;
        }
        }
    }

    self->mLoopingSoundHandle = 0;
    self->unk_630 = 0;
    self->mParticle2 = self->unk_630;
    self->unk_628 = self->mParticle2;

    *(State**)((char *)&self->unk_374) = *(State**)((char *)&self->unk_370);
    *(State**)((char *)&self->unk_370) = newState;

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

    *(unsigned short*)(((long long)(int)((char *)&self->mStateFlags))) &= ~0x200;

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
        State *s = *(State**)((char *)&self->unk_370);
        if (*(int*)s == 0) return 1;
        return (((C3*)((char *)self))->*(s->onEnter))();
    }
}
