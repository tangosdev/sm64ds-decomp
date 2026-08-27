//cpp
extern "C" {
extern void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void*, void*);
extern void func_ov015_02111fb8(void*, int);
void func_ov015_02111ce0(char* c){
  _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(c, 0);
  int v=*(int*)(c+0x320)+(int)0xffe16000;
  if(*(int*)(c+0x5c)>v) return;
  *(int*)(c+0x5c)=v;
  func_ov015_02111fb8(c, 0);
}
}
