extern void _ZN12WithMeshClsnD1Ev(void *);
extern void func_0207328c(void *p, int count, int size, void *dtor);
extern int func_ov002_020aed18(int *x);
extern void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *h);
extern char data_ov034_021144bc[];
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void);
extern void func_02011508(void);
extern void func_020072c0(void);
extern void _ZN15TextureSequenceD1Ev(void);
extern void _ZN15MaterialChangerD1Ev(void);
extern void _ZN9ModelAnimD1Ev(void);
extern void *_ZN6Memory11gameHeapPtrE;
int func_ov034_021112b0(char *c){
  *(void**)c = data_ov034_021144bc;
  _ZN12WithMeshClsnD1Ev(c+0x708);
  func_0207328c(c+0x5b8, 5, 0x40, (void*)_ZN25MovingCylinderClsnWithPosD1Ev);
  func_0207328c(c+0x478, 5, 0x40, (void*)_ZN25MovingCylinderClsnWithPosD1Ev);
  func_0207328c(c+0x444, 5, 6, (void*)func_02011508);
  func_0207328c(c+0x408, 5, 0xc, (void*)func_020072c0);
  func_0207328c(c+0x3cc, 5, 0xc, (void*)func_020072c0);
  func_0207328c(c+0x368, 5, 0x14, (void*)_ZN15TextureSequenceD1Ev);
  func_0207328c(c+0x304, 5, 0x14, (void*)_ZN15MaterialChangerD1Ev);
  func_0207328c(c+0x110, 5, 0x64, (void*)_ZN9ModelAnimD1Ev);
  func_ov002_020aed18((int*)c);
  _ZN6Memory10DeallocateEPvP4Heap(c, _ZN6Memory11gameHeapPtrE);
  return (int)c;
}
