//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x41c]; PMF *pp; };
extern "C" void func_ov070_02120d70(C *c) { PMF *p = c->pp; (c->**p)(); }
