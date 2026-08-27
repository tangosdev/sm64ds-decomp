//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; char rest[12]; };
extern Entry data_ov014_0211476c[];
struct C { char pad[0x610]; int idx; };
extern "C" void func_ov014_02111ebc(C *c, int i) {
  c->idx = i;
  int j = c->idx;
  (c->*data_ov014_0211476c[j].pmf)();
}
