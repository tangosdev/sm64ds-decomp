//cpp
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void*);
extern void _ZN10dCcAcPos_cD1Ev(void*);
extern void _ZN10dCcAcPos_cC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* _ZTV15RecRoomCupboard[];
int* RecRoomCupboard_Spawn(void){
  int* p = (int*)_ZN7fBase_cnwEj(0x21c);
  if(p){
    _ZN8dActor_cC2Ev(p);
    *(void***)p = (void**)_ZTV15RecRoomCupboard;
    func_020733a8((char*)p+0xd4, 5, 0x40, _ZN10dCcAcPos_cC1Ev, _ZN10dCcAcPos_cD1Ev);
  }
  return p;
}
}
