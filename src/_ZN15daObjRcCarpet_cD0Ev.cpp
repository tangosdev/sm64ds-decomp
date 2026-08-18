//cpp
// @symbol _ZN15daObjRcCarpet_cD0Ev
/* recovered: vtable identified, declarations from a shared header. Same wall as
 * src/_ZN15daObjRcCarpet_cD1Ev.cpp: hand-written under the mangled D0 name
 * rather than a real `delete` expression, matching include/daObjPathLift_c.h's
 * precedent for a PathLift-derived class. */
extern "C" {
extern void _ZN9ModelAnimD1Ev(void*);
extern void __destroy_arr(void* arr, int count, int size, void(*dtor)(void*));
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN8dActor_cD2Ev(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern void* _ZTV15daObjRcCarpet_c[];
extern void* data_ov002_0210af70[];
extern void* _ZTV10dBgActor_c[];
extern void* data_020a0eac;


void* _ZN15daObjRcCarpet_cD0Ev(char* p){
  *(void***)p = (void**)_ZTV15daObjRcCarpet_c;
  _ZN9ModelAnimD1Ev(p+0x450);
  *(void***)p = (void**)data_ov002_0210af70;
  __destroy_arr(p+0x320, 3, 0x50, _ZN5ModelD1Ev);
  *(void***)p = (void**)_ZTV10dBgActor_c;
  _ZN18MovingMeshColliderD1Ev(p+0x124);
  _ZN5ModelD1Ev(p+0xd4);
  _ZN8dActor_cD2Ev(p);
  _ZN6Memory10DeallocateEPvP4Heap(p, data_020a0eac);
  return p;
}
}
