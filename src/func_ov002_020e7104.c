extern unsigned int NEXT_ACTOR_UPDATE_FLAGS;
void func_ov002_020e7104(char* c, int r1){
  if(r1==0){
    *(unsigned short*)(((int)c + 0x4A2) & 0xFFFFFFFFFFFFFFFF) &= ~0x100;
    if(NEXT_ACTOR_UPDATE_FLAGS & 0x4000000) return;
    *(unsigned int*)(((int)c + 0xB0) & 0xFFFFFFFFFFFFFFFF) |= 0x4000000;
    NEXT_ACTOR_UPDATE_FLAGS |= 0x4000000;
    *(unsigned short*)((int)c + 0x496) = 0x64;
    return;
  }
  *(unsigned short*)(((int)c + 0x4A2) & 0xFFFFFFFFFFFFFFFF) |= 0x100;
}
