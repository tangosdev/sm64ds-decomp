//cpp
// @symbol func_ov018_021128e0
// @emits IceSheet_OnGroundPounded
/* recovered: renamed to Class_Method */
/* daObjIceBoard_c::OnGroundPounded - recovered from vtable slot identity */
struct C {
  virtual void p00(); virtual void p01(); virtual void p02(); virtual void p03();
  virtual void p04(); virtual void p05(); virtual void p06(); virtual void p07();
  virtual void p08(); virtual void p09(); virtual void p10(); virtual void p11();
  virtual void p12(); virtual void p13(); virtual void p14(); virtual void p15();
  virtual void p16(); virtual void p17(); virtual void p18(); virtual void p19();
  virtual void p20(); virtual void p21(); virtual void p22(); virtual void p23();
  virtual void p24(); virtual void p25(); virtual void p26(); virtual void p27();
  virtual void p28(); virtual void p29(); virtual void p30(); virtual void f7c();
};
struct A { char pad[8]; int kind; };
extern "C" void IceSheet_OnGroundPounded(C *c, A *a){
  if(a == 0) return;
  if(a->kind != 2) return;
  c->f7c();
}
