//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN5ActorC2Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void func_ov027_02112158(void*);
extern void func_ov027_021125a8(void*);
extern void* _ZTV13SnowmanBreath[];
int* SnowmanBreath_Spawn(void){
  int* p = (int*)_ZN9ActorBasenwEj(0x13d4);
  if(p){
    _ZN5ActorC2Ev(p);
    *(void***)p = (void**)_ZTV13SnowmanBreath;
    func_020733a8((char*)p+0xd4, 0x32, 0x60, func_ov027_021125a8, func_ov027_02112158);
  }
  return p;
}
}
