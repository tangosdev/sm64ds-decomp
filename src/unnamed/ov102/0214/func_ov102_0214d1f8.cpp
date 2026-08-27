//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x3ac]; PMF *pp; };
extern "C" int func_ov102_0214d1f8(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
