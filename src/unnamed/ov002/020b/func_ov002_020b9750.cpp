//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; int extra; };
extern Entry data_ov002_021097bc[];
struct C { char pad[0x3c0]; int idx; };
extern "C" void func_ov002_020b9750(C *c) {
  int j = c->idx;
  (c->*data_ov002_021097bc[j].pmf[1])();
}
