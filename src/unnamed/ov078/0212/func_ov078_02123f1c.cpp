//cpp
typedef int Fix12i;
struct C;
extern "C" int func_ov078_02123804(C* c);
extern "C" int _ZN6Player9GetHealthEv(void);
extern "C" void func_ov078_02123864(C* c);
extern "C" void func_ov002_020db54c(int self, int a, int b, int d);
extern "C" int _ZN9Animation8FinishedEv(void* anim);
extern "C" void KingBobOmb_SetState(C* c, void* p);
extern char data_ov078_0212709c[];

struct C {
    char pad0[0x8e];
    short field_8e;
    char pad90[0x31c-0x90];
    char anim_31c;
    char pad31d[0x430-0x31d];
    int field_430;
    char pad434[0x494-0x434];
    int field_494;
};

extern "C" int func_ov078_02123f1c(C* c)
{
    if (func_ov078_02123804(c) != 1) {
        if (c->field_494 == 0) goto L6c;
        if (_ZN6Player9GetHealthEv() != 0) goto L6c;
    }
    func_ov078_02123864(c);
    if (c->field_494 != 0) {
        func_ov002_020db54c(c->field_494, 0, 0x50000, c->field_8e);
        c->field_430 = c->field_494;
        c->field_494 = 0;
    }
    return 1;
L6c:
    if (_ZN9Animation8FinishedEv(&c->anim_31c)) {
        KingBobOmb_SetState(c, data_ov078_0212709c);
    }
    return 1;
}
