void* func_02054ea8(void){
  int off = ((*(volatile unsigned short*)0x400000a) & 0x3c) >> 2;
  unsigned int sbase = (*(volatile unsigned int*)0x4000000) & 0x7000000;
  return (void*)(0x6000000 + ((sbase >> 0x18) << 0x10) + (off << 14));
}
