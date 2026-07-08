extern int func_0207328c(void*, int, int, void*);
extern void _ZN12WithMeshClsnD1Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void _ZN11ShadowModelD1Ev(void*);
extern void _ZN9ModelAnimD1Ev(void*);
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ActorD1Ev(void*);
extern void* data_ov063_0211ed34;
extern void* _ZTV8Platform;
void* func_ov063_0211d4b8(void* c) {
  *(void**)c = &data_ov063_0211ed34;
  _ZN12WithMeshClsnD1Ev((char*)c+0x50c);
  func_0207328c((char*)c+0x48c, 2, 0x40, &_ZN25MovingCylinderClsnWithPosD1Ev);
  _ZN11ShadowModelD1Ev((char*)c+0x3d4);
  _ZN11ShadowModelD1Ev((char*)c+0x3ac);
  _ZN11ShadowModelD1Ev((char*)c+0x384);
  _ZN9ModelAnimD1Ev((char*)c+0x320);
  *(void**)c = &_ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev((char*)c+0x124);
  _ZN5ModelD1Ev((char*)c+0xd4);
  _ZN5ActorD1Ev(c);
  return c;
}
