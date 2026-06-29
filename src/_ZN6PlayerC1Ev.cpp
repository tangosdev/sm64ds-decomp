//cpp
extern "C" {
extern int data_ov002_0210a83c[];
void _ZN5ActorC2Ev(void*);
void _ZN9ModelAnimC1Ev(void*);
void func_020733a8(void*,int,int,void*,void*);
void _ZN15TextureSequenceD1Ev(void*);
void _ZN15TextureSequenceC1Ev(void*);
void _ZN15MaterialChangerD1Ev(void*);
void _ZN15MaterialChangerC1Ev(void*);
void _ZN11ShadowModelC1Ev(void*);
void _ZN25MovingCylinderClsnWithPosC1Ev(void*);
void _ZN12WithMeshClsnC1Ev(void*);
void* _ZN6PlayerC1Ev(void* c){
  char* p=(char*)c;
  _ZN5ActorC2Ev(c);
  char* r4=p+0xdc;
  *(int*)p=(int)data_ov002_0210a83c;
  _ZN9ModelAnimC1Ev(r4+0x14);
  _ZN9ModelAnimC1Ev(r4+0x98);
  func_020733a8(r4+0x100,4,0x14,(void*)_ZN15TextureSequenceC1Ev,(void*)_ZN15TextureSequenceD1Ev);
  func_020733a8(r4+0x150,2,0x14,(void*)_ZN15MaterialChangerC1Ev,(void*)_ZN15MaterialChangerD1Ev);
  func_020733a8(r4+0x178,2,0x14,(void*)_ZN15TextureSequenceC1Ev,(void*)_ZN15TextureSequenceD1Ev);
  _ZN11ShadowModelC1Ev(p+0x2ac);
  _ZN25MovingCylinderClsnWithPosC1Ev(p+0x2d4);
  _ZN25MovingCylinderClsnWithPosC1Ev(p+0x314);
  _ZN12WithMeshClsnC1Ev(p+0x380);
  return c;
}
}
