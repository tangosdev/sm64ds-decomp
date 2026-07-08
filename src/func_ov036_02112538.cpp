//cpp
extern "C" {
void* _ZN9ActorBasenwEj(unsigned int);
void _ZN8PlatformC2Ev(void*);
void func_020733a8(void*, int, int, void*, void*);
void _ZN7PathPtrC1Ev(void*);
void _ZN9ModelAnimC1Ev(void*);
extern int _ZTV8PathLift[];
extern int data_ov036_02113f9c[];
void _ZN5ModelD1Ev(void*);
void _ZN5ModelC1Ev(void*);
void* func_ov036_02112538(void){
  char* c = (char*)_ZN9ActorBasenwEj(0x4c0);
  if(c){
    _ZN8PlatformC2Ev(c);
    *(int*)c = (int)_ZTV8PathLift;
    func_020733a8(c+0x320, 3, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    _ZN7PathPtrC1Ev(c+0x430);
    *(int*)c = (int)data_ov036_02113f9c;
    _ZN9ModelAnimC1Ev(c+0x450);
  }
  return c;
}
}
