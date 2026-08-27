//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3f0]; PMF *pp; };
extern "C" void func_ov077_02124718(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
