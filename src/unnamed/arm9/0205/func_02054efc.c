void* func_02054efc(void){
  int off = ((*(volatile unsigned short*)0x4000008) & 0x3c) >> 2;
  unsigned int sbase = (*(volatile unsigned int*)0x4000000) & 0x7000000;
  return (void*)(0x6000000 + ((sbase >> 0x18) << 0x10) + (off << 14));
}
