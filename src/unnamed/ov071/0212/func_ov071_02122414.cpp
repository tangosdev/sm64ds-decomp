//cpp
struct C; typedef void (C::*PMF)();
struct Entry { char pad[8]; PMF pmf; char tail[20 - 8 - sizeof(PMF)]; };
extern Entry data_ov071_02122ecc[];
struct C { char pad[0x320]; int idx; };
extern "C" void func_ov071_02122414(C *c) {
  int j = c->idx;
  (c->*data_ov071_02122ecc[j].pmf)();
}
