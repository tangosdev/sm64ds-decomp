extern int data_ov055_02111ae0[];
extern int _ZN15TextureSequenceD1Ev(void*);
extern int data_020a0eac[];
extern int func_0207328c(void*,int,int,void*);
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN5ActorD1Ev(void*);
extern int _ZN6Memory10DeallocateEPvP4Heap(void*,void*);
int func_ov055_021111f8(char* c){
  *(int*)c=(int)data_ov055_02111ae0;
  func_0207328c(c+0x1b0,2,0x14,(void*)_ZN15TextureSequenceD1Ev);
  _ZN11ShadowModelD1Ev(c+0x188);
  _ZN5ModelD1Ev(c+0x138);
  _ZN9ModelAnimD1Ev(c+0xd4);
  _ZN5ActorD1Ev(c);
  _ZN6Memory10DeallocateEPvP4Heap(c,(void*)data_020a0eac[0]);
  return (int)c;
}
