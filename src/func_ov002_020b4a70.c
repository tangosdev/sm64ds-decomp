extern int __destroy_arr(void*, int, int, void*);
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN5ActorD2Ev(void*);
extern int _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern void* _ZTV13daObjDorifu_c;
extern void* _ZTV8Platform;
extern void* data_020a0eac;
void* func_ov002_020b4a70(void* c) {
  *(void**)c = &_ZTV13daObjDorifu_c;
  __destroy_arr((char*)c+0x4b0, 5, 0x1c8, &_ZN18MovingMeshColliderD1Ev);
  __destroy_arr((char*)c+0x320, 5, 0x50, &_ZN5ModelD1Ev);
  *(void**)c = &_ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev((char*)c+0x124);
  _ZN5ModelD1Ev((char*)c+0xd4);
  _ZN5ActorD2Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, *(void**)&data_020a0eac);
  return c;
}
