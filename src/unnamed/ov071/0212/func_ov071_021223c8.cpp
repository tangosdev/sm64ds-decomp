//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; int extra[3]; };
extern Entry data_ov071_02122ecc[];
struct C { char pad[0x320]; int idx; };
extern "C" void func_ov071_021223c8(C *c, int i) {
  c->idx = i;
  int j = c->idx;
  (c->*data_ov071_02122ecc[j].pmf)();
}
