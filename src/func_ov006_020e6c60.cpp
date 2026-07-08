//cpp
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int func_ov004_020b29c0(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
extern int data_ov006_0213c62c[];
extern void* _ZN6Memory11gameHeapPtrE[];
int func_ov006_020e6c60(char* c){
  *(int*)c=(int)data_ov006_0213c62c;
  _ZN8Particle10SysTrackerD1Ev(c+0x47e4);
  func_ov004_020b29c0(c);
  _ZN6Memory10DeallocateEPvP4Heap(c,_ZN6Memory11gameHeapPtrE[0]);
  return (int)c;
}
}
