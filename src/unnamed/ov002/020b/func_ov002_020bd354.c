extern unsigned short data_0209b274;
int func_ov002_020bd354(char* c, int a, int b){
  if(b == data_0209b274){
    *(unsigned char*)(c+0x6ff) = 1;
    if(*(unsigned short*)(c+0x6ae) == 0)
      *(unsigned short*)(c+0x6ae) = 0x198;
  }
  *(int*)(c+0xa8) = -0x20000;
  *(int*)(c+0x684) = *(int*)(c+0x60);
  return 1;
}
