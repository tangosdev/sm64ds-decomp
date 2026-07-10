//cpp
extern "C" {
extern void _ZN9ModelAnimD1Ev(void*);
extern void func_0207328c(void* arr, int count, int size, void(*dtor)(void*));
extern void _ZN18MovingMeshColliderD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ActorD1Ev(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern void* data_ov036_02113f9c[];
extern void* data_ov002_0210af70[];
extern void* _ZTV17ExclamationSwitch[];
extern void* data_020a0eac;


void* func_ov036_021121c8(char* p){
  *(void***)p = (void**)data_ov036_02113f9c;
  _ZN9ModelAnimD1Ev(p+0x450);
  *(void***)p = (void**)data_ov002_0210af70;
  func_0207328c(p+0x320, 3, 0x50, _ZN5ModelD1Ev);
  *(void***)p = (void**)_ZTV17ExclamationSwitch;
  _ZN18MovingMeshColliderD1Ev(p+0x124);
  _ZN5ModelD1Ev(p+0xd4);
  _ZN5ActorD1Ev(p);
  _ZN6Memory10DeallocateEPvP4Heap(p, data_020a0eac);
  return p;
}
}
