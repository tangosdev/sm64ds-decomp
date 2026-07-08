extern int func_0207328c(void*, int, int, void*);
extern int _ZN18MovingMeshColliderD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN5ActorD1Ev(void*);
extern int _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern void* data_ov002_02108d94;
extern void* _ZTV8Platform;
extern void* _ZN6Memory11gameHeapPtrE;
void* func_ov002_020b4a70(void* c) {
  *(void**)c = &data_ov002_02108d94;
  func_0207328c((char*)c+0x4b0, 5, 0x1c8, &_ZN18MovingMeshColliderD1Ev);
  func_0207328c((char*)c+0x320, 5, 0x50, &_ZN5ModelD1Ev);
  *(void**)c = &_ZTV8Platform;
  _ZN18MovingMeshColliderD1Ev((char*)c+0x124);
  _ZN5ModelD1Ev((char*)c+0xd4);
  _ZN5ActorD1Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, *(void**)&_ZN6Memory11gameHeapPtrE);
  return c;
}
