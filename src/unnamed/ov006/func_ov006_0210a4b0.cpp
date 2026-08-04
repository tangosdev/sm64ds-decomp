//cpp
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int func_ov004_020b29c0(void*);
extern int data_ov006_0213e448[];
int func_ov006_0210a4b0(char* c){
  *(int*)c=(int)data_ov006_0213e448;
  _ZN8Particle10SysTrackerD1Ev(c+0x471c);
  func_ov004_020b29c0(c);
  return (int)c;
}
}
