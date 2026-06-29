//cpp
struct Base {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
  virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18();
  virtual int m_4c(void* arg);   /* slot 19 -> 0x4c */
};
struct Obj {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void m_c(int a, int b);   /* slot 3 -> 0xc */
  virtual void m_10(int a, int b);  /* slot 4 -> 0x10 */
  virtual void v5();
  virtual int m_18();               /* slot 6 -> 0x18 */
  virtual int m_1c();               /* slot 7 -> 0x1c */
};
extern "C" void func_ov004_020b0aa0(int arg);
extern "C" void func_ov004_020ae20c(void);
extern "C" void func_ov004_020ae2c8(void);
extern "C" void func_ov004_020b29a0(Base* c, void* arg);
extern Obj* data_0209f5bc;
extern Base* data_ov004_020beb68;

extern "C" void func_ov004_020b8560(char* c){
  Base* r4 = data_ov004_020beb68;
  if (data_0209f5bc->m_18()) {
    data_0209f5bc->m_c(0x1e, 0);
    func_ov004_020b0aa0(0x1d);
    func_ov004_020ae20c();
    func_ov004_020ae2c8();
    func_ov004_020b29a0(r4, *(void**)(c + 0x18));
    *(int*)(c + 0x1c) = 0;
    *(int*)(c + 0x18) = -1;
    *(int*)(c + 0x20) = 0;
    *(int*)(c + 0x24) = 0;
  } else {
    if (*(int*)(c + 0x1c) != 0)
      return;
    if (data_0209f5bc->m_1c())
      return;
    if (r4 == 0)
      return;
    if (r4->m_4c(*(void**)(c + 0x18)) == 0)
      return;
    data_0209f5bc->m_10(0x1e, 0);
  }
}
