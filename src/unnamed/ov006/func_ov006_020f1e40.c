void func_ov006_020f1e40(unsigned *base, int idx){
  unsigned *p = base + idx;
  *(unsigned*)((unsigned char*)p + 0x4bb8) = 0;
  *(unsigned*)((unsigned char*)p + 0x4d98) = 0;
}
