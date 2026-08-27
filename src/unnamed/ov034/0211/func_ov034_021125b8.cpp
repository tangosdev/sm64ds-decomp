//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; char pad[12]; };
extern Entry data_ov034_02114538[];
struct C { char pad[0x8c4]; int idx; };
extern "C" void func_ov034_021125b8(C *c, int i) {
  c->idx = i;
  int j = c->idx;
  (c->*data_ov034_02114538[j].pmf)();
}
