//cpp
struct B {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
  virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18();
  virtual int m(int a);
};
extern "C" {
extern void func_ov004_020b0aa0(int arg);
extern void func_ov004_020b29a0(void* c, int arg);
extern B* data_ov004_020beb68;
void func_ov004_020b78f4(char* c) {
  B* r4;
  if (*(int*)(c + 0x1c) != 0) return;
  r4 = data_ov004_020beb68;
  if (r4 == 0) return;
  if (r4->m(*(int*)(c + 0x18)) == 0) return;
  func_ov004_020b0aa0(0x1d);
  func_ov004_020b29a0(r4, *(int*)(c + 0x18));
  *(int*)(c + 0x1c) = 0;
  *(int*)(c + 0x18) = -1;
  *(int*)(c + 0x20) = 0;
  *(int*)(c + 0x24) = 0;
}
}
