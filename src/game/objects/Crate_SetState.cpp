//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov098_0213c878[];
struct C { char pad[0x560]; int idx; };
extern "C" void Crate_SetState(C *c, int i) { c->idx = i; int j = c->idx; (c->*data_ov098_0213c878[j].pmf[0])(); }
