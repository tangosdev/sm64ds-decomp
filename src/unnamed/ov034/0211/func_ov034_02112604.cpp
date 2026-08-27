//cpp
struct C; typedef void (C::*PMF)();
struct Entry { char pad[8]; PMF pmf; char tail[20 - 8 - sizeof(PMF)]; };
extern Entry data_ov034_02114538[];
struct C { char pad[0x8c4]; int idx; };
extern "C" void func_ov034_02112604(C *c) {
  int j = c->idx;
  (c->*data_ov034_02114538[j].pmf)();
}
