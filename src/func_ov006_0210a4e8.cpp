//cpp
// @symbol func_ov006_0210a4e8
/* recovered: renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int _ZN11dScMgBase_cD2Ev(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
extern int data_ov006_0213e448[];
extern void* data_020a0eac[];
int func_ov006_0210a4e8(char* c){
  *(int*)c=(int)data_ov006_0213e448;
  _ZN8Particle10SysTrackerD1Ev(c+0x471c);
  _ZN11dScMgBase_cD2Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c,data_020a0eac[0]);
  return (int)c;
}
}
