//cpp
extern "C" {
int func_0207328c(void*, int, int, void*);
int _ZN11ShadowModelD1Ev(void*);
int _ZN9ModelAnimD1Ev(void*);
int _ZN12WithMeshClsnD1Ev(void*);
int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
int func_ov002_020aed18(void*);
int _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern int _ZTV14UnchainedChomp[];
extern void func_02011508();
extern void func_020072c0();
extern void _ZN5ModelD1Ev();
extern int data_020a0eac[];
void* _ZN14UnchainedChompD0Ev(char* c){
  *(int**)c = _ZTV14UnchainedChomp;
  func_0207328c(c+0x768, 6, 6, (void*)func_02011508);
  func_0207328c(c+0x720, 6, 0xc, (void*)func_020072c0);
  func_0207328c(c+0x6d8, 6, 0xc, (void*)func_020072c0);
  _ZN11ShadowModelD1Ev(c+0x640);
  func_0207328c(c+0x550, 6, 0x28, (void*)_ZN11ShadowModelD1Ev);
  func_0207328c(c+0x370, 6, 0x50, (void*)_ZN5ModelD1Ev);
  _ZN9ModelAnimD1Ev(c+0x30c);
  _ZN12WithMeshClsnD1Ev(c+0x150);
  _ZN25MovingCylinderClsnWithPosD1Ev(c+0x110);
  func_ov002_020aed18(c);
  _ZN6Memory10DeallocateEPvP4Heap(c, (void*)data_020a0eac[0]);
  return c;
}
}
