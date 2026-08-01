//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov064_0211c98c[];
struct C { char pad[0x16c]; int idx; };
extern "C" void func_ov064_0211a734(C *c) { int j = c->idx; (c->*data_ov064_0211c98c[j].pmf[1])(); }
