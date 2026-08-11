//cpp
// @symbol func_ov006_020e3528
// recovered name: dScMgCurling_c_Behavior
/* recovered: renamed to Class_Method */
/* dScMgCurling_c::Behavior - recovered from vtable slot identity */
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[1]; };
extern Entry data_ov006_02141950[];
struct C { char pad[0x4eac]; int idx; };
extern "C" int func_ov006_020e12d0(C*);
extern "C" int func_ov006_020e3528(C* c){
  int j = c->idx;
  (c->*data_ov006_02141950[j].pmf[0])();
  func_ov006_020e12d0(c);
  return 1;
}
