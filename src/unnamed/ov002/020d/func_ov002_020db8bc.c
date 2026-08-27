void func_ov002_020db8bc(unsigned char *p, unsigned char val){
  p[0x6f2] = 1;
  *(unsigned short*)(p + 0x6b2) = 0;
  p[0x6f3] = val;
}
