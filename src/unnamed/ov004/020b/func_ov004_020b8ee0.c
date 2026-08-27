int func_ov004_020b8ee0(char* p){
  int i;
  int n = *(int*)(p+0x120);
  for (i = 0; i < n; i++) {
    if (*(int*)(p+0xc) != 0) return 0;
    p += 0x24;
  }
  return 1;
}
