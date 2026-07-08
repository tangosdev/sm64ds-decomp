//cpp
struct Obj {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void m_c(int a, int b);   /* slot 3 -> 0xc */
  virtual void v4(); virtual void v5();
  virtual int m_18();               /* slot 6 -> 0x18 */
};
struct Base;
extern "C" void func_ov004_020b0aa0(int arg);
extern "C" void func_ov004_020ae20c(void);
extern "C" void func_ov004_020ae2c8(void);
extern "C" void func_ov004_020b29a0(void* c, void* arg);
extern Obj* SCENE_FADER;
extern void* data_ov004_020beb68;

extern "C" void func_ov004_020b7fec(char* c){
  if (SCENE_FADER->m_18() == 0)
    return;
  void* r5 = data_ov004_020beb68;
  if (r5 == 0)
    return;
  SCENE_FADER->m_c(0x1e, 0);
  func_ov004_020b0aa0(0x1d);
  func_ov004_020ae20c();
  func_ov004_020ae2c8();
  func_ov004_020b29a0(r5, *(void**)(c + 0x18));
  *(int*)(c + 0x1c) = 0;
  *(int*)(c + 0x18) = -1;
  *(int*)(c + 0x20) = 0;
  *(int*)(c + 0x24) = 0;
}
