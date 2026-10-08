void func_ov007_020b7384(char *c){
  extern void func_020566dc(const void *src, unsigned int offset, unsigned int count);
  extern void func_02056674(const void *src, unsigned int offset, unsigned int count);
  func_020566dc(*(const void**)(c+0x10), 0, 0x400);
  func_02056674(*(const void**)(c+0x14), 0, 0x400);
}
