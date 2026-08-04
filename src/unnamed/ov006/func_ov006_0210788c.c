extern void _ZN5ModelD1Ev(void*);
extern int __destroy_arr(void*, int, int, void*);
extern void func_ov006_020c1c64(void*);
extern void _ZN8Particle10SysTrackerD1Ev(void*);
extern void func_ov004_020b29c0(void*);
extern int data_ov006_0213e39c;
extern int data_ov006_0213e448;
extern void func_ov006_021079c8(void);
void* func_ov006_0210788c(char* p){
  *(int*)p = (int)&data_ov006_0213e39c;
  _ZN5ModelD1Ev(p + 0x536c);
  _ZN5ModelD1Ev(p + 0x531c);
  __destroy_arr(p + 0x51a8, 5, 0x34, (void*)func_ov006_021079c8);
  func_ov006_020c1c64(p + 0x4f38);
  *(int*)p = (int)&data_ov006_0213e448;
  _ZN8Particle10SysTrackerD1Ev(p + 0x471c);
  func_ov004_020b29c0(p);
  return p;
}
