//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x31c]; PMF *pp; };
extern "C" void func_ov070_02121fd0(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
