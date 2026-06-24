//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN5ActorC2Ev(void*);
extern void _ZN9ModelAnimC1Ev(void*);
extern void _ZN12WithMeshClsnC1Ev(void*);
extern void _ZN18MovingCylinderClsnC1Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void func_ov065_021180b8(void*);
extern void func_ov065_021196bc(void*);
extern void* data_ov065_0211ce48[];

int* func_ov065_02119634(void){
  int* p = (int*)_ZN9ActorBasenwEj(0x11b8);
  if(p){
    _ZN5ActorC2Ev(p);
    *(void***)p = (void**)data_ov065_0211ce48;
    _ZN9ModelAnimC1Ev((char*)p+0xec);
    func_020733a8((char*)p+0x150, 7, 0x200, func_ov065_021196bc, func_ov065_021180b8);
    _ZN12WithMeshClsnC1Ev((char*)p+0xf50);
    _ZN18MovingCylinderClsnC1Ev((char*)p+0x110c);
    _ZN25MovingCylinderClsnWithPosC1Ev((char*)p+0x1140);
  }
  return p;
}
}
