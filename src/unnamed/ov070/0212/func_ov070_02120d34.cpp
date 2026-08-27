//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x41c]; PMF *pp; };
extern "C" void func_ov070_02120d34(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
