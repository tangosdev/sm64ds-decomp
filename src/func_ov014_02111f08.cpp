//cpp
struct C; typedef void (C::*PMF)();
struct Entry { char pad[8]; PMF pmf; char tail[20 - 8 - sizeof(PMF)]; };
extern Entry data_ov014_0211476c[];
struct C { char pad[0x610]; int idx; };
extern "C" void func_ov014_02111f08(C *c) {
  int j = c->idx;
  (c->*data_ov014_0211476c[j].pmf)();
}
