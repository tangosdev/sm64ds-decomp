//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x380]; PMF *pp; };
extern "C" void func_ov071_021202b4(C *c) { PMF *p = c->pp; (c->**p)(); }
