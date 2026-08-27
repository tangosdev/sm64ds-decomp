//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov084_02130dc4[];
struct C { char pad[0x1e4]; int idx; };
extern "C" void func_ov084_0212c9a8(C *c) { int j = c->idx; (c->*data_ov084_02130dc4[j].pmf[1])(); }
