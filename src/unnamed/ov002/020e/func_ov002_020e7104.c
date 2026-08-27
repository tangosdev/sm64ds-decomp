extern unsigned int data_0209b454;
void func_ov002_020e7104(char* c, int r1){
  if(r1==0){
    *(unsigned short*)(((int)c + 0x4A2) & 0xFFFFFFFFFFFFFFFF) &= ~0x100;
    if(data_0209b454 & 0x4000000) return;
    *(unsigned int*)(((int)c + 0xB0) & 0xFFFFFFFFFFFFFFFF) |= 0x4000000;
    data_0209b454 |= 0x4000000;
    *(unsigned short*)((int)c + 0x496) = 0x64;
    return;
  }
  *(unsigned short*)(((int)c + 0x4A2) & 0xFFFFFFFFFFFFFFFF) |= 0x100;
}
