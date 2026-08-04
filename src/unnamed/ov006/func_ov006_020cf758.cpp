//cpp
struct C;
typedef void (C::*PMF)();
struct C { PMF pmf; };
extern "C" void func_ov006_020cf758(C *c) {
  (c->*(c->pmf))();
}
