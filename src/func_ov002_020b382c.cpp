//cpp
// @symbol func_ov002_020b382c
// @emits BigBrickBlock_OnGroundPounded
/* recovered: renamed to Class_Method */
/* daObjBlockL_c::OnGroundPounded - recovered from vtable slot identity */
struct Arg { char pad[8]; int f8; };
struct Obj {
  virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
  virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
  virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
  virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
  virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
  virtual void v28(); virtual void v29(); virtual void v30(); virtual void target();
};
extern "C" void BigBrickBlock_OnGroundPounded(Obj* o, Arg* a) {
  if (a->f8 == 3) return;
  int b = (*(unsigned short*)((char*)o+0xc) == 0x11);
  if (b != 0) {
    if (a->f8 != 2) return;
    o->target();
  } else {
    o->target();
  }
}
