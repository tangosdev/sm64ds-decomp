//cpp
struct C; typedef void (C::*PMF)();
extern PMF data_ov004_020beb88[];
struct C { char pad[0x1e]; short idx; };
extern "C" void func_ov004_020adf2c(C *c) {
  (c->*data_ov004_020beb88[c->idx])();
}
