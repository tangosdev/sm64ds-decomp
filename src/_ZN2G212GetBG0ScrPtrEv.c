void* _ZN2G212GetBG0ScrPtrEv(void){
  int off = ((*(volatile unsigned short*)0x4000008) & 0x1f00) >> 8;
  unsigned int sbase = (*(volatile unsigned int*)0x4000000) & 0x38000000;
  return (void*)(0x6000000 + ((sbase >> 0x1b) << 0x10) + (off << 11));
}
