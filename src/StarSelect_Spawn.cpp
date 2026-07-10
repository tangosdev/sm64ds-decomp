//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN9ActorBaseC1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* data_0208e4b8[];
extern void* _ZTV5Stage[];
extern void* data_ov003_020b1704[];

void* StarSelect_Spawn(void){
  char* p = (char*)_ZN9ActorBasenwEj(0x13c);
  if(p){
    _ZN9ActorBaseC1Ev(p);
    *(void***)p = (void**)data_0208e4b8;
    *(void***)p = (void**)_ZTV5Stage;
    unsigned char* fl = (unsigned char*)(((int)p + 0x13) & 0xFFFFFFFFFFFFFFFF);
    *fl |= 1;
    *fl |= 4;
    *(void***)p = (void**)data_ov003_020b1704;
    func_020733a8(p+0x64, 2, 0x50, _ZN5ModelC1Ev, _ZN5ModelD1Ev);
  }
  return p;
}
}
