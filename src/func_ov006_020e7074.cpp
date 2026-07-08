//cpp
extern "C" {
extern int func_ov004_020b0620(void*);
extern unsigned int FRAME_TIMER;
int _ZN8Particle10SysTracker6UpdateEv(void*);
int func_ov006_020e7074(void* c){
  if(func_ov004_020b0620(c)==0) return 0;
  if(FRAME_TIMER & 1)
    _ZN8Particle10SysTracker6UpdateEv((char*)c+0x47e4);
  return 1;
}
}
