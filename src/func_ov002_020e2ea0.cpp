//cpp
#include "dBgCh_Actr.h"

extern "C" {
struct dActor_c {
    static dActor_c* FindWithID(unsigned int id);
};
struct Player {
    struct State {};
    int IsState(State &s);
    void ChangeState(State &s);
};

int func_ov002_020e3078(Player *self, Player::State *s);
dBgPi *_ZNK10dBgCh_Actr14GetFloorResultEv(const dBgCh_Actr *self);
}

extern signed char data_0209f2f8;
extern Player::State data_ov002_021105bc;
extern Player::State data_ov002_021105a4;
extern Player::State data_ov002_021101e4;
extern Player::State data_ov002_0211040c;

extern "C" int func_ov002_020e2ea0(Player *self) {
    char *base = (char *)self;
    signed char c = data_0209f2f8;

    if (c == 0x13 || c == 0xa || c == 0x10) {
        /* proceed */
    } else if (c != 0x22) {
        goto ret0;
    }

    int val = *(int *)(base + 0x664);
    if (val == 1 || val == 0xe) {
        return 0;
    }

    if (!((dBgCh_Actr *)(base + 0x380))->IsOnGround()) {
        return 0;
    }

    dBgPi *cr = _ZNK10dBgCh_Actr14GetFloorResultEv((dBgCh_Actr *)(base + 0x380));
    if (cr->GetClsnID() != -1) {
        if (dActor_c::FindWithID((unsigned int)cr->GetClsnID()) != 0) {
            return 0;
        }
    }

    int diff = *(int *)(base + 0x684) - *(int *)(base + 0x60);
    if (diff <= 0x3e8000) {
        goto ret0;
    }
    if (*(int *)(base + 0x558) < 0xddb) {
        goto ret0;
    }

    *(unsigned char *)(base + 0x6e3) = 0;
    if (self->IsState(data_ov002_021105bc)) {
        *(unsigned char *)(base + 0x6e3) = 1;
    } else if (self->IsState(data_ov002_021105a4)) {
        *(unsigned char *)(base + 0x6e3) = 2;
    } else {
        if (func_ov002_020e3078(self, &data_ov002_021101e4)) {
            *(short *)(base + 0x8e) = *(short *)(base + 0x94);
        }
    }

    *(unsigned char *)(base + 0x70c) = 0;
    if (c == 0x10) {
        *(unsigned char *)(base + 0x70c) = 1;
    } else if (c == 0x22) {
        *(unsigned char *)(base + 0x70c) = 2;
    }

    self->ChangeState(data_ov002_0211040c);
    return 1;

ret0:
    return 0;
}
