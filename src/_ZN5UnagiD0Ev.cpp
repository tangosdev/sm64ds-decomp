//cpp
extern "C" {
extern int func_0207328c(void*,int,int,void*);
extern void _ZN14BlendModelAnimD1Ev(void*);
extern void _ZN12WithMeshClsnD1Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int func_ov002_020aed18(int*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
extern int _ZTV5Unagi[];
extern void func_020072c0(void);
extern void* data_020a0eac[];
void* _ZN5UnagiD0Ev(char* c){
  *(int**)c = _ZTV5Unagi;
  func_0207328c(c+0x448, 7, 0xc, (void*)func_020072c0);
  _ZN14BlendModelAnimD1Ev(c+0x350);
  _ZN12WithMeshClsnD1Ev(c+0x190);
  _ZN25MovingCylinderClsnWithPosD1Ev(c+0x150);
  _ZN25MovingCylinderClsnWithPosD1Ev(c+0x110);
  func_ov002_020aed18((int*)c);
  _ZN6Memory10DeallocateEPvP4Heap(c, data_020a0eac[0]);
  return c;
}
}
