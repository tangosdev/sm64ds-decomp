//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3a4]; PMF *pp; };
extern "C" void func_ov030_02114170(C *c) { PMF *p = c->pp; (c->**p)(); }
