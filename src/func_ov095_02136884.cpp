//cpp
extern "C" {
extern int data_ov095_021376f0[];
extern int func_020072c0[];
extern int _ZN6Memory11gameHeapPtrE[];
extern int _ZN19CylinderClsnWithPosD1Ev[];
void func_0207328c(void*, int, int, void*);
void _ZN5ActorD1Ev(void*);
void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
void* func_ov095_02136884(char* c){
  *(int*)c = (int)data_ov095_021376f0;
  func_0207328c(c+0x3a4, 0xc, 0xc, (void*)func_020072c0);
  func_0207328c(c+0xd4, 0xc, 0x3c, (void*)_ZN19CylinderClsnWithPosD1Ev);
  _ZN5ActorD1Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, (void*)_ZN6Memory11gameHeapPtrE[0]);
  return c;
}
}
