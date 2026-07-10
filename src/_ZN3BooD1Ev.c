extern int _ZTV3Boo[];
extern int _ZN11ShadowModelD1Ev(void*);
extern int _ZN5ModelD1Ev(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int* func_ov002_020aedbc(int*);
int _ZN3BooD1Ev(char* c){
  *(int*)c=(int)_ZTV3Boo;
  _ZN11ShadowModelD1Ev(c+0x45c);
  _ZN11ShadowModelD1Ev(c+0x434);
  _ZN5ModelD1Ev(c+0x3e4);
  _ZN9ModelAnimD1Ev(c+0x380);
  _ZN12WithMeshClsnD1Ev(c+0x1c4);
  _ZN25MovingCylinderClsnWithPosD1Ev(c+0x184);
  func_ov002_020aedbc((int*)c);
  return (int)c;
}
