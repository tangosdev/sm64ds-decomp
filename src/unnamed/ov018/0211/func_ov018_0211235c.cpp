//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x370]; PMF *pp; };
extern "C" void func_ov018_0211235c(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
