//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x35c]; PMF *pp; };
extern "C" void func_ov072_02121d18(C *c) { PMF *p = c->pp; (c->**p)(); }
