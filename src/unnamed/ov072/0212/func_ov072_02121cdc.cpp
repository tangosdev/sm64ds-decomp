//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x35c]; PMF *pp; };
extern "C" void func_ov072_02121cdc(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
