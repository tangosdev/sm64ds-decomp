extern int data_ov003_020b1704[];
extern void _ZN5ModelD1Ev(void*);
extern void func_0207328c(void*, int, int, void*);
extern int _ZTV5Scene[];
extern int _ZTV12ActorDerived[];
extern void _ZN9ActorBaseD1Ev(void*);
extern void* _ZN6Memory11gameHeapPtrE;
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
int* func_ov003_020ade54(int* t){
  t[0]=(int)data_ov003_020b1704;
  func_0207328c((char*)t+0x64, 2, 0x50, (void*)_ZN5ModelD1Ev);
  t[0]=(int)_ZTV5Scene;
  t[0]=(int)_ZTV12ActorDerived;
  _ZN9ActorBaseD1Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, _ZN6Memory11gameHeapPtrE);
  return t;
}
