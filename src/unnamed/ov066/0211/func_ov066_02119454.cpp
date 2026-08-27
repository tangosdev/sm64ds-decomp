//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x48c]; PMF *pp; };
extern "C" int func_ov066_02119454(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
