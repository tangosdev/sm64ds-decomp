extern int data_ov075_0211d304[];
extern void func_ov075_02115bc8(void);
extern void func_ov075_02115bac(void);
extern int _ZTV5Scene[];
extern int _ZTV12ActorDerived[];
extern void func_0207328c(void*, int, int, void*);
extern void _ZN9ActorBaseD1Ev(void*);
extern void* _ZN6Memory11gameHeapPtrE;
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
int* func_ov075_02115b28(int* t){
  t[0]=(int)data_ov075_0211d304;
  func_0207328c((char*)t+0x1b4, 4, 0x2c, (void*)func_ov075_02115bc8);
  func_0207328c((char*)t+0x70, 9, 0x24, (void*)func_ov075_02115bac);
  t[0]=(int)_ZTV5Scene;
  t[0]=(int)_ZTV12ActorDerived;
  _ZN9ActorBaseD1Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, _ZN6Memory11gameHeapPtrE);
  return t;
}
