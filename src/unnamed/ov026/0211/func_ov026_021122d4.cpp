//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x30c]; PMF *pp; };
extern "C" int func_ov026_021122d4(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
