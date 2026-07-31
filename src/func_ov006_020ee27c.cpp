//cpp
// @symbol func_ov006_020ee27c
// @emits dScMgJump_c_Behavior
/* recovered: renamed to Class_Method */
/* dScMgJump_c::Behavior - recovered from vtable slot identity */
struct C; typedef void (C::*PMF)();
struct C { char pad[0x5004]; PMF m; };
extern "C" int dScMgJump_c_Behavior(C* c) { (c->*c->m)(); return 1; }
