//cpp
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int func_ov004_020b29c0(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
extern int data_ov006_0213e448[];
extern void* data_020a0eac[];
int func_ov006_0210a4e8(char* c){
  *(int*)c=(int)data_ov006_0213e448;
  _ZN8Particle10SysTrackerD1Ev(c+0x471c);
  func_ov004_020b29c0(c);
  _ZN6Memory10DeallocateEPvP4Heap(c,data_020a0eac[0]);
  return (int)c;
}
}
