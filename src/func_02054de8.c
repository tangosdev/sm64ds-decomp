unsigned func_02054de8(void){
  int v1 = *(volatile int*)0x4001000;
  unsigned short v2 = *(volatile unsigned short*)0x400100c;
  if (!((v1 & 7) >= 5 && (v2 & 0x80)))
    return ((unsigned)(v2 & 0x3c) >> 2 << 0xe) + 0x6200000;
  return 0;
}
