//cpp
struct C; typedef int (C::*PMF)(int);
struct C { char pad[0x140]; PMF *pp; };
extern "C" int func_ov100_021453d8(C *c, PMF *p, int a2) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(a2); }
