//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3cc]; PMF *pp; };
extern "C" void func_ov027_02111cfc(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
