extern void func_ov002_020bd984(char* c, int a);
void func_ov002_020d80d0(char* c){
  if (*(unsigned char*)(c+0x6f8) != 1) return;
  *(short*)(c+0x6be) = 0;
  *(unsigned char*)(c+0x6f8) = 0;
  *(unsigned char*)(c+0x6f4) = 0;
  *(unsigned char*)(c+0x714) = 0;
  func_ov002_020bd984(c, 0x32);
}
