//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN5ActorC2Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* _ZTV4Tree[];
int* Tree_Spawn(void){
  int* p = (int*)_ZN9ActorBasenwEj(0x264);
  if(p){
    _ZN5ActorC2Ev(p);
    *(void***)p = (void**)_ZTV4Tree;
    func_020733a8((char*)p+0xd4, 5, 0x50, _ZN5ModelC1Ev, _ZN5ModelD1Ev);
  }
  return p;
}
}
