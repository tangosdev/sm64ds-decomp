//cpp
extern "C" {
extern void* _ZN9ActorBasenwEj(unsigned int);
extern void _ZN8PlatformC2Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern int func_020733a8(void*,int,int,void*,void*);
extern void* _ZTV11PyramidLift;
extern void _ZN7Vector3D1Ev(void);
extern void func_0203d384(void);
void* PyramidLift_Spawn(void){
  char* c=(char*)_ZN9ActorBasenwEj(0x3fc);
  if(c){
    _ZN8PlatformC2Ev(c);
    *(void**)c=&_ZTV11PyramidLift;
    _ZN5ModelC1Ev(c+0x320);
    func_020733a8(c+0x37c, 0xa, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);
  }
  return c;
}
}
