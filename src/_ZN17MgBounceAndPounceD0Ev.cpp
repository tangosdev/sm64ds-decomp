//cpp
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int func_ov004_020b29c0(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
extern int _ZTV17MgBounceAndPounce[];
extern void* data_020a0eac[];
int _ZN17MgBounceAndPounceD0Ev(char* c){
  *(int*)c=(int)_ZTV17MgBounceAndPounce;
  _ZN8Particle10SysTrackerD1Ev(c+0x47e4);
  func_ov004_020b29c0(c);
  _ZN6Memory10DeallocateEPvP4Heap(c,data_020a0eac[0]);
  return (int)c;
}
}
