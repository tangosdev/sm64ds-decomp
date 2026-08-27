//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov102_0214e890[];
struct C { char pad[0x3e8]; int idx; };
extern "C" void func_ov102_02149df0(C *c) { int j = c->idx; (c->*data_ov102_0214e890[j].pmf[1])(); }
