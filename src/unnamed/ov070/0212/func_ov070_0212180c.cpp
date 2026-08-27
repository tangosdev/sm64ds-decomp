//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x39c]; PMF *pp; };
extern "C" void func_ov070_0212180c(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
