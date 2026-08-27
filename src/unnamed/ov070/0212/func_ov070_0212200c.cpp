//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x31c]; PMF *pp; };
extern "C" void func_ov070_0212200c(C *c) { PMF *p = c->pp; (c->**p)(); }
