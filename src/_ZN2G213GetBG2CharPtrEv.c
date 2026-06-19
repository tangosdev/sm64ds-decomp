void* _ZN2G213GetBG2CharPtrEv(void){
  int mode = *(volatile unsigned int*)0x4000000 & 7;
  unsigned int bg2cnt = *(volatile unsigned short*)0x400000c;
  if(mode < 5 || !(bg2cnt & 0x80))
    return (void*)(0x6000000
      + (((*(volatile unsigned int*)0x4000000 & 0x7000000) >> 0x18) << 0x10)
      + (((bg2cnt & 0x3c) >> 2) << 14));
  return (void*)0;
}
