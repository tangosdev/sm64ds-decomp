//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov002_02110a5c[];
struct C { char pad[0x3f0]; int idx; };
extern "C" void func_ov002_020ed63c(C *c, int i) { c->idx = i; int j = c->idx; (c->*data_ov002_02110a5c[j].pmf[0])(); }
