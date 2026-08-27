//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3cc]; PMF *pp; };
extern "C" void func_ov027_02111d38(C *c) { PMF *p = c->pp; (c->**p)(); }
