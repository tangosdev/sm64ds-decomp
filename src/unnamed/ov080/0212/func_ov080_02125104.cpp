//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x36c]; PMF *pp; };
extern "C" void func_ov080_02125104(C *c) { PMF *p = c->pp; (c->**p)(); }
