struct P2 { int a, b; };
struct VT {
  void (*v0)(void*); void (*v1)(void*); void (*v2)(void*);
  void (*m_c)(void*, int, int);   /* 0xc */
  void (*m_10)(void*, int, int);  /* 0x10 */
  void (*v5)(void*);
  int (*m_18)(void*);             /* 0x18 */
  int (*m_1c)(void*);             /* 0x1c */
};
struct Obj { struct VT* vt; };

extern int func_ov004_020b0b1c(int arg);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void _ZN5Scene9SetFadersEP15FaderBrightness(void* fb);
extern void func_0202ec9c(void* thiz, int arg1);
extern void _ZN5Scene14StartSceneFadeEjjt(unsigned int a, unsigned int b, unsigned short c);
extern void func_02012dd0(int c);

extern struct Obj* SCENE_FADER;
extern char data_0209f61c[];
extern struct P2 data_ov004_020bc8dc;
extern int data_ov004_020bc8a8;
extern int data_ov004_020bc898;
extern int data_ov004_020bc86c;
extern int data_ov004_020bc8a4;

void func_ov004_020b8098(char* r4){
  struct Obj* o;
  if (func_ov004_020b0b1c(0)) {
    o = SCENE_FADER;
    o->vt->m_10(o, 0x1e, 0);
    *(struct P2*)(r4 + 8) = data_ov004_020bc8dc;
    return;
  }
  if (func_ov004_020b0b1c(2) || func_ov004_020b0b1c(1)) {
    o = SCENE_FADER;
    if (o->vt->m_1c(o))
      return;
    _ZN5Scene9SetFadersEP15FaderBrightness(data_0209f61c);
    func_0202ec9c(data_0209f61c, 2);
    _ZN5Scene14StartSceneFadeEjjt(5, 0, 0);
    func_02012dd0(0x3c);
    return;
  }
  if (*(int*)(r4 + 0x1c) != 0)
    return;
  func_ov004_020b0cac(0, data_ov004_020bc8a8, data_ov004_020bc898, -1, -1, 0xd);
  func_ov004_020b0cac(2, data_ov004_020bc86c, data_ov004_020bc8a4, -1, -1, 0xd);
}
