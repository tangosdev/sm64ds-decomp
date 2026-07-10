extern int func_0207328c(void* p, int a, int b, void* d);
extern void _ZN11ShadowModelD1Ev(void* p);
extern void _ZN14BlendModelAnimD1Ev(void* p);
extern void _ZN12WithMeshClsnD1Ev(void* p);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void* p);
extern int func_ov002_020aed18(int* x);
extern int func_020072c0;
extern int _ZTV11ChiefChilly[];
int _ZN11ChiefChillyD1Ev(int* c){
  *(int**)c = _ZTV11ChiefChilly;
  func_0207328c((char*)c+0x4d4, 2, 0xc, &func_020072c0);
  func_0207328c((char*)c+0x448, 8, 0xc, &func_020072c0);
  func_0207328c((char*)c+0x3e8, 8, 0xc, &func_020072c0);
  _ZN11ShadowModelD1Ev((char*)c+0x380);
  _ZN14BlendModelAnimD1Ev((char*)c+0x30c);
  _ZN12WithMeshClsnD1Ev((char*)c+0x150);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)c+0x110);
  func_ov002_020aed18(c);
  return (int)c;
}
