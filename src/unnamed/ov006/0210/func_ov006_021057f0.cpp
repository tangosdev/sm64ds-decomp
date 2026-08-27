//cpp
struct C;
typedef void (C::*PMF)();
extern PMF data_ov006_02142820[];
struct B {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
  virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
  virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
  virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
  virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
  virtual void v32(); virtual void v33(); virtual void v34();
  virtual int m();
};
struct C : B { char pad[0x4fdc]; unsigned char idx; };
extern "C" void func_ov006_02104c60(C*);
extern "C" void func_ov006_021057f0(C *c){
  if (c->m() != 0) return;
  (c->*data_ov006_02142820[c->idx])();
  func_ov006_02104c60(c);
}
