extern void* func_ov007_020c2c68(int a, int b, int c);
void func_ov007_020c2b9c(int count, int* src){
  int flag = 0;
  void* r;
  if (src != 0) flag |= 1;
  r = func_ov007_020c2c68(0, count, flag);
  if ((flag & 1) != 0) {
    int i;
    for (i = 0; i < count; i++) {
      ((int*)(*(int**)((char*)r+0x20)))[i] = src[i];
    }
  }
  *(unsigned short*)((char*)r+8) = (unsigned short)count;
}
