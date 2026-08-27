//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x3dc]; PMF *pp; };
extern "C" void func_ov081_02127708(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
