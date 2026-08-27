//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x1e4]; PMF *pp; };
extern "C" void func_ov071_021215c0(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
