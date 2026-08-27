//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3dc]; PMF *pp; };
extern "C" void func_ov081_02127744(C *c) { PMF *p = c->pp; (c->**p)(); }
