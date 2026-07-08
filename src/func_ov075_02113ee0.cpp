//cpp
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN9ModelAnimD1Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN8Particle10SysTrackerD1Ev(void*);
extern void _ZN9ActorBaseD1Ev(void*);
extern int data_ov075_0211c6a0[];
extern int func_ov075_02113fdc[];
extern int _ZTV12ActorDerived[];
void* func_ov075_02113ee0(char* c){
  *(int**)c = data_ov075_0211c6a0;
  func_0207328c(c+0x920, 4, 0x158, func_ov075_02113fdc);
  _ZN9ModelAnimD1Ev(c+0x8bc);
  _ZN5ModelD1Ev(c+0x86c);
  _ZN8Particle10SysTrackerD1Ev(c+0x50);
  *(int**)c = _ZTV12ActorDerived;
  _ZN9ActorBaseD1Ev(c);
  return c;
}
}
