//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x370]; PMF *pp; };
extern "C" void func_ov019_021122a4(C *c) { PMF *p = c->pp; (c->**p)(); }
