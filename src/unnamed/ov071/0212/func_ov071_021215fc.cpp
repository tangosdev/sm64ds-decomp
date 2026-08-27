//cpp
struct C; typedef void (C::*PMF)();
struct C { char pad[0x1e4]; PMF *pp; };
extern "C" void func_ov071_021215fc(C *c) { PMF *p = c->pp; (c->**p)(); }
