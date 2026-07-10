extern void* _ZN9ActorBasenwEj(unsigned int sz);
extern void func_020aed98(void);
extern int func_020733a8(void *p, int a, int b, void *ctor, void *dtor);
extern int _ZN12WithMeshClsnC1Ev(void *p);
extern int _ZN25MovingCylinderClsnWithPosC1Ev(void *p);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void *p);
extern int func_0203d73c(void *p);
extern int func_02011508(void *p);
extern int func_0203d384(void *p);
extern int func_020072c0(void *p);
extern int _ZN15TextureSequenceC1Ev(void *p);
extern int _ZN15TextureSequenceD1Ev(void *p);
extern int _ZN15MaterialChangerC1Ev(void *p);
extern int _ZN15MaterialChangerD1Ev(void *p);
extern int _ZN9ModelAnimC1Ev(void *p);
extern int _ZN9ModelAnimD1Ev(void *p);
extern int _ZTV7Wiggler[];

void *Wiggler_Spawn(void){
  char *c = (char *)_ZN9ActorBasenwEj(0x8e8);
  if(c){
    func_020aed98();
    *(int**)(c) = _ZTV7Wiggler;
    func_020733a8(c+0x110, 5, 0x64, _ZN9ModelAnimC1Ev, _ZN9ModelAnimD1Ev);
    func_020733a8(c+0x304, 5, 0x14, _ZN15MaterialChangerC1Ev, _ZN15MaterialChangerD1Ev);
    func_020733a8(c+0x368, 5, 0x14, _ZN15TextureSequenceC1Ev, _ZN15TextureSequenceD1Ev);
    func_020733a8(c+0x3cc, 5, 0xc, func_0203d384, func_020072c0);
    func_020733a8(c+0x408, 5, 0xc, func_0203d384, func_020072c0);
    func_020733a8(c+0x444, 5, 6, func_0203d73c, func_02011508);
    func_020733a8(c+0x478, 5, 0x40, _ZN25MovingCylinderClsnWithPosC1Ev, _ZN25MovingCylinderClsnWithPosD1Ev);
    func_020733a8(c+0x5b8, 5, 0x40, _ZN25MovingCylinderClsnWithPosC1Ev, _ZN25MovingCylinderClsnWithPosD1Ev);
    _ZN12WithMeshClsnC1Ev(c+0x708);
  }
  return c;
}
