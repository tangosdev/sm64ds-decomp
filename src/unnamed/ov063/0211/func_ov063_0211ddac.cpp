//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov063_0211efbc[];
struct C { char pad[0x6c8]; int idx; };
extern "C" void func_ov063_0211ddac(C *c, int i) { c->idx = i; int j = c->idx; (c->*data_ov063_0211efbc[j].pmf[0])(); }
