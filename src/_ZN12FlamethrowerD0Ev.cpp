//cpp
extern "C" {
extern int _ZTV12Flamethrower[];
extern int func_020072c0[];
extern int data_020a0eac[];
extern int _ZN19CylinderClsnWithPosD1Ev[];
void func_0207328c(void*, int, int, void*);
void _ZN5ActorD1Ev(void*);
void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
void* _ZN12FlamethrowerD0Ev(char* c){
  *(int*)c = (int)_ZTV12Flamethrower;
  func_0207328c(c+0x3a4, 0xc, 0xc, (void*)func_020072c0);
  func_0207328c(c+0xd4, 0xc, 0x3c, (void*)_ZN19CylinderClsnWithPosD1Ev);
  _ZN5ActorD1Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, (void*)data_020a0eac[0]);
  return c;
}
}
