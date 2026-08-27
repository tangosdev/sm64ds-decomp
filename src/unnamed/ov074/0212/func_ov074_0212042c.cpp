//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov074_021230f8[];
struct C { char pad[0x5cc]; int idx; };
extern "C" void func_ov074_0212042c(C *c) { int j = c->idx; (c->*data_ov074_021230f8[j].pmf[1])(); }
