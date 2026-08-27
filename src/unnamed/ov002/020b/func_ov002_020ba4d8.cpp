//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov002_0210e00c[];
struct C { char pad[0x340]; int idx; };
extern "C" void func_ov002_020ba4d8(C *c, int i) { c->idx = i; int j = c->idx; (c->*data_ov002_0210e00c[j].pmf[0])(); }
