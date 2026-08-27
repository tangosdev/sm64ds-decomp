//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x320]; PMF *pp; };
extern "C" int func_ov073_021223a4(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
