//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x384]; PMF *pp; };
extern "C" void func_ov096_021368b4(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
