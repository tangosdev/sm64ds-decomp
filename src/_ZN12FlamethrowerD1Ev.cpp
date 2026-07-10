//cpp
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN5ActorD1Ev(void*);
extern int _ZTV12Flamethrower[];
extern int func_020072c0[];
extern int _ZN19CylinderClsnWithPosD1Ev();
int _ZN12FlamethrowerD1Ev(char* c){
  *(int**)c = _ZTV12Flamethrower;
  func_0207328c((char*)c+0x3a4, 0xc, 0xc, (void*)func_020072c0);
  func_0207328c((char*)c+0xd4, 0xc, 0x3c, (void*)&_ZN19CylinderClsnWithPosD1Ev);
  _ZN5ActorD1Ev(c);
  return (int)c;
}
}
