extern void* func_ov007_020c3df4(int a, int b);
void func_ov007_020c13a4(char *c, int r1, short r2, short r3){
  *(void**)(c) = func_ov007_020c3df4(0, r1*4);
  *(void**)(c+4) = func_ov007_020c3df4(0, r1*2);
  *(int*)(c+0xc) = r1;
  *(short*)(c+8) = r2;
  *(short*)(c+0xa) = r3;
}
