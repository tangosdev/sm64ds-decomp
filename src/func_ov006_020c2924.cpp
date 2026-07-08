//cpp
extern "C" {
extern int func_ov006_020c2848(void* c);
extern void* SCENE_FADER;
void func_ov006_020c2924(char* c){
  func_ov006_020c2848(c);
  void* obj=SCENE_FADER;
  int (**vt)(void*)=*(int(***)(void*))obj;
  if(vt[6](obj)==0) return;
  *(int*)(c+0x190)=0;
  *(int*)(c+0x18c)=*(int*)(c+0x190);
  *(int*)(c+0x198)=0;
  *(int*)(c+0x194)=*(int*)(c+0x198);
  *(int*)(c+0x60)=0;
  *(int*)(c+0xd0)=0;
  *(int*)(c+0xe4)=0;
}
}
