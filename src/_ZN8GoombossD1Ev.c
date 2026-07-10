extern int func_0207328c(void* p, int a, int b, void* d);
extern void _ZN12WithMeshClsnD1Ev(void* p);
extern void _ZN18TextureTransformerD1Ev(void* p);
extern void _ZN15TextureSequenceD1Ev(void* p);
extern void _ZN15MaterialChangerD1Ev(void* p);
extern void _ZN9ModelAnimD1Ev(void* p);
extern int func_ov002_020aed18(int* x);
extern int func_020072c0;
extern int _ZN11ShadowModelD1Ev;
extern int _ZN25MovingCylinderClsnWithPosD1Ev;
extern int _ZTV8Goomboss[];
int _ZN8GoombossD1Ev(int* c){
  *(int**)c = _ZTV8Goomboss;
  _ZN12WithMeshClsnD1Ev((char*)c+0x40c);
  _ZN18TextureTransformerD1Ev((char*)c+0x3f8);
  _ZN15TextureSequenceD1Ev((char*)c+0x3e4);
  _ZN15MaterialChangerD1Ev((char*)c+0x3d0);
  func_0207328c((char*)c+0x3ac, 3, 0xc, &func_020072c0);
  func_0207328c((char*)c+0x274, 3, 0x28, &_ZN11ShadowModelD1Ev);
  _ZN9ModelAnimD1Ev((char*)c+0x210);
  func_0207328c((char*)c+0x110, 4, 0x40, &_ZN25MovingCylinderClsnWithPosD1Ev);
  func_ov002_020aed18(c);
  return (int)c;
}
