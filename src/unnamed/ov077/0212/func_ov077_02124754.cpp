//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3f0]; PMF *pp; };
extern "C" void func_ov077_02124754(C *c) { PMF *p = c->pp; (c->**p)(); }
