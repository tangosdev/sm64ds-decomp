//cpp
struct Obj {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void m_c(int a, int b);   /* slot 3 -> 0xc */
  virtual void m_10(int a, int b);  /* slot 4 -> 0x10 */
  virtual void v5();
  virtual int m_18();               /* slot 6 -> 0x18 */
  virtual int m_1c();               /* slot 7 -> 0x1c */
};
extern "C" int func_ov004_020b0b1c(int arg);
extern "C" void _ZN5Scene9SetFadersEP15FaderBrightness(void* p);
extern "C" void func_0202ec9c(void* thiz, int arg1);
extern "C" void _ZN5Scene14StartSceneFadeEjjt(unsigned int a, unsigned int b, unsigned short c);
extern "C" void func_02012dd0(void* c);
extern "C" void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);

extern Obj* SCENE_FADER;
struct Pair { int a; int b; };
extern Pair data_ov004_020bc914;
extern void* data_0209f61c;
extern int data_ov004_020bc8a8;
extern int data_ov004_020bc898;
extern int data_ov004_020bc86c;
extern int data_ov004_020bc8a4;

extern "C" void func_ov004_020b72d4(char* c){
  if (*(int*)(c + 0x1c) == 0) {
    if (func_ov004_020b0b1c(0) != 0) {
      SCENE_FADER->m_10(0x1e, 0);
      int a = data_ov004_020bc914.a;
      int b = data_ov004_020bc914.b;
      *(int*)(c + 8) = b ? a : a;
      *(int*)(c + 0xc) = b;
      return;
    }
    if (func_ov004_020b0b1c(2) == 0) {
      if (func_ov004_020b0b1c(1) == 0)
        return;
    }
    if (SCENE_FADER->m_1c() != 0)
      return;
    _ZN5Scene9SetFadersEP15FaderBrightness(&data_0209f61c);
    func_0202ec9c(&data_0209f61c, 2);
    _ZN5Scene14StartSceneFadeEjjt(5, 0, 0);
    func_02012dd0((void*)0x3c);
    return;
  }
  func_ov004_020b0cac(0, data_ov004_020bc8a8, data_ov004_020bc898, -1, -1, 0xd);
  func_ov004_020b0cac(2, data_ov004_020bc86c, data_ov004_020bc8a4, -1, -1, 0xd);
  *(int*)(c + 0x1c) = 0;
}
