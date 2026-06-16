//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; char rest[12]; };
extern Entry data_ov085_0212fe88[];
struct C { char pad[0x1fc]; int idx; };
extern "C" void func_ov085_02129524(C *c, int i) {
  c->idx = i;
  int j = c->idx;
  (c->*data_ov085_0212fe88[j].pmf)();
}
