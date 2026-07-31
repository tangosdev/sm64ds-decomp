//cpp
// @symbol func_ov080_02126c60
// @emits daPicGate_c_Behavior
/* recovered: renamed to Class_Method */
/* daPicGate_c::Behavior - recovered from vtable slot identity */
struct C;
typedef int (C::*PMF)();
struct Entry { char pad[0x8]; PMF pmf; };
struct C { char pad[0x1a4]; Entry *ep; };
extern "C" int daPicGate_c_Behavior(C *c) {
  (c->*c->ep->pmf)();
  return 1;
}
