//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3d0]; PMF *pp; };
extern "C" void func_ov077_02125e5c(C *c) { PMF *p = c->pp; (c->**p)(); }
