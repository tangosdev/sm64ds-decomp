//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3d0]; PMF *pp; };
extern "C" void func_ov077_02125e20(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
