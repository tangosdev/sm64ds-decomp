//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x42c]; PMF *pp; };
extern "C" int func_ov062_0211c658(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
