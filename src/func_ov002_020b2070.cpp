//cpp
extern "C" {
extern void DecIfAbove0_Short(void*);
extern void DecIfAbove0_Byte(void*);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
void func_ov002_020b2070(char* c){
  DecIfAbove0_Short(c+0x3a8);
  DecIfAbove0_Byte(c+0x3aa);
  if(*(unsigned short*)(c+0x3a8)==1)
    _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}
}
