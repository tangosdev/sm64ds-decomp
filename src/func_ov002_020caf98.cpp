//cpp
typedef unsigned char u8;
typedef unsigned short u16;

struct Ret { int pad; int y; };

struct Arg {
    virtual void *m0();
    virtual void *m1();
    virtual struct Ret *m2();
};

struct Player {
    struct State {};

    int IsState(State &s);
    int IsInAir();
    int IsBeingShotOutOfCannon();
    int ChangeState(State &s);
};

extern Player::State data_ov002_0211013c;
extern Player::State data_ov002_021105bc;
extern Player::State data_ov002_0211031c;
extern Player::State data_ov002_021101b4;
extern Player::State data_ov002_021106dc;

extern "C" int func_ov002_020e0478(void *c);
extern "C" int func_ov002_020e3078(struct Player *self, void *s);

extern "C" int func_ov002_020caf98(Player *self, Arg *arg)
{
    char *p = (char *)self;
    int b354, b358;

    if (self->IsState(data_ov002_0211013c) == 0 &&
        *(u16 *)(p + 0x6b6) == 0)
    {
        b354 = (*(int *)(p + 0x354) != 0);
        if (b354 == 0 &&
            *(int *)(p + 0x37c) == 0)
        {
            b358 = (*(int *)(p + 0x358) != 0);
            if (b358 == 0 &&
                *(u8 *)(p + 0x708) == 0 &&
                *(u8 *)(p + 0x709) == 0 &&
                *(u8 *)(p + 0x706) == 0 &&
                *(u8 *)(p + 0x703) == 0)
            {
                goto cont;
            }
        }
    }
    return 0;

cont:
    if (self->IsInAir() != 0) goto state_check;
    if (self->IsBeingShotOutOfCannon() != 0) goto state_check;
    if (self->IsState(data_ov002_021105bc) != 0) goto state_check;
    if (self->IsState(data_ov002_0211031c) != 0) goto state_check;

    if (func_ov002_020e0478(self) == 0) goto ret0a;

state_check:
    if (self->IsState(data_ov002_021101b4) != 0) {
        if (func_ov002_020e3078(self, &data_ov002_021106dc) != 0) return 0;
    }

    if (*(int *)((char *)arg + 0x18) & 0x400000) return 0;

    {
        Ret *r = arg->m2();
        if (*(int *)(p + 0x60) - r->y < -0x64000)
            *(int *)(p + 0x60) = r->y - 0x64000;
    }

    *(void **)(p + 0x37c) = arg;
    self->ChangeState(data_ov002_021106dc);
    return 1;

ret0a:
    return 0;
}
