//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x370]; PMF *pp; };
extern "C" void func_ov019_02112268(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
