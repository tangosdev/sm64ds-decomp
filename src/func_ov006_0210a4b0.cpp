//cpp
extern "C" {
extern int _ZN8Particle10SysTrackerD1Ev(void*);
extern int _ZN11dScMgBase_cD2Ev(void*);
extern int data_ov006_0213e448[];
int func_ov006_0210a4b0(char* c){
  *(int*)c=(int)data_ov006_0213e448;
  _ZN8Particle10SysTrackerD1Ev(c+0x471c);
  _ZN11dScMgBase_cD2Ev(c);
  return (int)c;
}
}
