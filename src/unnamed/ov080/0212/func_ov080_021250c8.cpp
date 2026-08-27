//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x36c]; PMF *pp; };
extern "C" void func_ov080_021250c8(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
