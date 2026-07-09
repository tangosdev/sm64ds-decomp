//cpp
struct Base {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
  virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18();
  virtual int m(void* arg);
};
extern "C" void func_ov004_020b0aa0(int arg);
extern "C" void func_ov004_020ae20c(void);
extern "C" void func_ov004_020ae2c8(void);
extern "C" void func_ov004_020b29a0(Base* c, void* arg);
extern Base* data_ov004_020beb68;

extern "C" void func_ov004_020b77b4(char* c){
  if (*(int*)(c + 0x1c) != 0)
    return;
  Base* o = data_ov004_020beb68;
  if (o == 0)
    return;
  if (o->m(*(void**)(c + 0x18)) == 0)
    return;
  func_ov004_020b0aa0(0x1d);
  func_ov004_020ae20c();
  func_ov004_020ae2c8();
  func_ov004_020b29a0(o, *(void**)(c + 0x18));
  *(int*)(c + 0x1c) = 0;
  *(int*)(c + 0x18) = -1;
  *(int*)(c + 0x20) = 0;
  *(int*)(c + 0x24) = 0;
}
