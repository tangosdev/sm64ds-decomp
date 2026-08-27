//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; int extra[3]; };
extern Entry data_ov002_021097bc[];
struct C { char pad[0x3c0]; int idx; };
extern "C" void func_ov002_020b9704(C *c, int i) {
  c->idx = i;
  int j = c->idx;
  (c->*data_ov002_021097bc[j].pmf)();
}
