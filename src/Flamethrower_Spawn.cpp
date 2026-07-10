//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN5ActorC2Ev(void*);
extern void _ZN19CylinderClsnWithPosD1Ev(void*);
extern void _ZN19CylinderClsnWithPosC1Ev(void*);
extern void func_020072c0(void*);
extern void func_0203d384(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* _ZTV12Flamethrower[];

int* Flamethrower_Spawn(void){
  int* p = (int*)_ZN9ActorBasenwEj(0x46c);
  if(p){
    _ZN5ActorC2Ev(p);
    *(void***)p = (void**)_ZTV12Flamethrower;
    func_020733a8((char*)p+0xd4, 0xc, 0x3c, _ZN19CylinderClsnWithPosC1Ev, _ZN19CylinderClsnWithPosD1Ev);
    func_020733a8((char*)p+0x3a4, 0xc, 0xc, func_0203d384, func_020072c0);
  }
  return p;
}
}
