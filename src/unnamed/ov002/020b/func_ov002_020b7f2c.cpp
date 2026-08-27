//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x3bc]; PMF *pp; };
extern "C" int func_ov002_020b7f2c(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 0; return (c->**q)(); }
