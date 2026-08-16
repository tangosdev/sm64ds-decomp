extern void __destroy_arr(void*, int, int, void*);
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ActorD2Ev(void*);
extern void* _ZTV13daObjDorifu_c;
extern void* _ZTV8Platform;
void* func_ov002_020b4af8(void* c) {
  *(void**)c = &_ZTV13daObjDorifu_c;
  __destroy_arr((char*)c+0x4b0, 5, 0x1c8, (void*)&_ZN18MovingMeshColliderD1Ev);
  __destroy_arr((char*)c+0x320, 5, 0x50, (void*)&_ZN5ModelD1Ev);
  *(void**)c = &_ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev((char*)c+0x124);
  _ZN5ModelD1Ev((char*)c+0xd4);
  _ZN5ActorD2Ev(c);
  return c;
}
