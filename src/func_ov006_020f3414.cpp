//cpp
// @symbol func_ov006_020f3414
// recovered name: dScMgLuigi_c_Behavior
/* recovered: renamed to Class_Method */
/* dScMgLuigi_c::Behavior - recovered from vtable slot identity */
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02142234[];
struct C { char pad[0x4f78]; int idx; };
extern "C" int func_ov006_020f3414(C* c) {
    int j = c->idx;
    (c->*data_ov006_02142234[j].pmf)();
    return 1;
}
