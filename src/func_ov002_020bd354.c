extern unsigned short KS_FRAME_COUNTER;
int func_ov002_020bd354(char* c, int a, int b){
  if(b == KS_FRAME_COUNTER){
    *(unsigned char*)(c+0x6ff) = 1;
    if(*(unsigned short*)(c+0x6ae) == 0)
      *(unsigned short*)(c+0x6ae) = 0x198;
  }
  *(int*)(c+0xa8) = -0x20000;
  *(int*)(c+0x684) = *(int*)(c+0x60);
  return 1;
}
