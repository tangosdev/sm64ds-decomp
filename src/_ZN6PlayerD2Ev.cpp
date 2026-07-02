//cpp
extern "C" {
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int _ZN11ShadowModelD1Ev(void*);
extern int func_0207328c(void*, int, int, void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN5ActorD1Ev(void*);
extern void _ZN15TextureSequenceD1Ev(void);
extern void _ZN15MaterialChangerD1Ev(void);
extern void* data_ov002_0210a83c[];
void* _ZN6PlayerD2Ev(void* c) {
  *(void***)c = data_ov002_0210a83c;
  _ZN12WithMeshClsnD1Ev((char*)c+0x380);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)c+0x314);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)c+0x2d4);
  _ZN11ShadowModelD1Ev((char*)c+0x2ac);
  func_0207328c((char*)c+0x254, 2, 0x14, (void*)_ZN15TextureSequenceD1Ev);
  func_0207328c((char*)c+0x22c, 2, 0x14, (void*)_ZN15MaterialChangerD1Ev);
  func_0207328c((char*)c+0x1dc, 4, 0x14, (void*)_ZN15TextureSequenceD1Ev);
  _ZN9ModelAnimD1Ev((char*)c+0x174);
  _ZN9ModelAnimD1Ev((char*)c+0xf0);
  _ZN5ActorD1Ev(c);
  return c;
}
}
