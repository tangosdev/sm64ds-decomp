//cpp
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void*);
extern void _ZN9ModelAnimC1Ev(void*);
extern void _ZN10dBgCh_ActrC1Ev(void*);
extern void _ZN7dCcAc_cC1Ev(void*);
extern void _ZN10dCcAcPos_cC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void func_ov065_021180b8(void*);
extern void func_ov065_021196bc(void*);
extern void* _ZTV6Dorrie[];

int* Dorrie_Spawn(void){
  int* p = (int*)_ZN7fBase_cnwEj(0x11b8);
  if(p){
    _ZN8dActor_cC2Ev(p);
    *(void***)p = (void**)_ZTV6Dorrie;
    _ZN9ModelAnimC1Ev((char*)p+0xec);
    func_020733a8((char*)p+0x150, 7, 0x200, func_ov065_021196bc, func_ov065_021180b8);
    _ZN10dBgCh_ActrC1Ev((char*)p+0xf50);
    _ZN7dCcAc_cC1Ev((char*)p+0x110c);
    _ZN10dCcAcPos_cC1Ev((char*)p+0x1140);
  }
  return p;
}
}
