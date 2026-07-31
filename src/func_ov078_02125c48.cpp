//cpp
struct C; typedef int (C::*PMF)();
struct C { char pad[0x420]; PMF *pp; };
extern "C" int func_ov078_02125c48(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
