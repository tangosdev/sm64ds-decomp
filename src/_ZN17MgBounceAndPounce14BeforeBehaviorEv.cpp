//cpp
extern "C" {
extern int func_ov004_020b0620(void*);
extern unsigned int data_020a0db0;
int _ZN8Particle10SysTracker6UpdateEv(void*);
int _ZN17MgBounceAndPounce14BeforeBehaviorEv(void* c){
  if(func_ov004_020b0620(c)==0) return 0;
  if(data_020a0db0 & 1)
    _ZN8Particle10SysTracker6UpdateEv((char*)c+0x47e4);
  return 1;
}
}
