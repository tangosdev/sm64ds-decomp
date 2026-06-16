//cpp
struct C; typedef void (C::*PMF)();
struct Entry { char pad[8]; PMF pmf; char tail[0x14 - 8 - sizeof(PMF)]; };
extern Entry data_ov085_0212fe88[];
struct C { char pad[0x1fc]; int idx; };
extern "C" void func_ov085_02129570(C *c) {
  int j = c->idx;
  (c->*data_ov085_0212fe88[j].pmf)();
}
