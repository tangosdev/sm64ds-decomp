//cpp
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void*);
extern void _ZN19CylinderClsnWithPosD1Ev(void*);
extern void _ZN19CylinderClsnWithPosC1Ev(void*);
extern void _ZN7Vector3D1Ev(void*);
extern void func_0203d384(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* _ZTV12Flamethrower[];

int* Flamethrower_Spawn(void){
  int* p = (int*)_ZN7fBase_cnwEj(0x46c);
  if(p){
    _ZN8dActor_cC2Ev(p);
    *(void***)p = (void**)_ZTV12Flamethrower;
    func_020733a8((char*)p+0xd4, 0xc, 0x3c, _ZN19CylinderClsnWithPosC1Ev, _ZN19CylinderClsnWithPosD1Ev);
    func_020733a8((char*)p+0x3a4, 0xc, 0xc, func_0203d384, _ZN7Vector3D1Ev);
  }
  return p;
}
}
