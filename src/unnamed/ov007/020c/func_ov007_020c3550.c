extern void* func_ov007_020c3df4(int a, int b);

void func_ov007_020c3550(int a, int b, int c, int d){
  char* p = (char*)func_ov007_020c3df4(0, 0xc);
  *(int*)(p) = a;
  *(unsigned char*)(p+8) = (unsigned char)b;
  *(unsigned char*)(p+9) = (unsigned char)c;
  *(int*)(p+4) = d;
  *(unsigned char*)(p+0xa) = 0;
}
