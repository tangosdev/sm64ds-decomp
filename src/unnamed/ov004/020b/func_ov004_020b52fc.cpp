//cpp
struct C;
typedef void (C::*PMF)();
struct C { PMF pmf; };
extern "C" void func_ov004_020b52fc(C *c) {
  (c->*(c->pmf))();
}
