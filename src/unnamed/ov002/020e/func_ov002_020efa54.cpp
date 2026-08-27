//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; char pad[0xc]; };
extern Entry data_ov002_0210af2c[];
struct C { char pad[0x44c]; int idx; };
extern "C" void func_ov002_020efa54(C* c, int i) {
    c->idx = i;
    int j = c->idx;
    (c->*data_ov002_0210af2c[j].pmf)();
}
