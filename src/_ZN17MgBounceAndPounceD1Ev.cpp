//cpp
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int func_ov004_020b29c0(void*);
extern int _ZTV17MgBounceAndPounce[];
int _ZN17MgBounceAndPounceD1Ev(char* c){
  *(int*)c=(int)_ZTV17MgBounceAndPounce;
  _ZN8Particle10SysTrackerD1Ev(c+0x47e4);
  func_ov004_020b29c0(c);
  return (int)c;
}
}
