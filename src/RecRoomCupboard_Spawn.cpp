//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN5ActorC2Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern void _ZN25MovingCylinderClsnWithPosC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* _ZTV15RecRoomCupboard[];
int* RecRoomCupboard_Spawn(void){
  int* p = (int*)_ZN9ActorBasenwEj(0x21c);
  if(p){
    _ZN5ActorC2Ev(p);
    *(void***)p = (void**)_ZTV15RecRoomCupboard;
    func_020733a8((char*)p+0xd4, 5, 0x40, _ZN25MovingCylinderClsnWithPosC1Ev, _ZN25MovingCylinderClsnWithPosD1Ev);
  }
  return p;
}
}
