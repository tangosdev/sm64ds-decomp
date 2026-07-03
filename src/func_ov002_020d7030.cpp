//cpp
extern "C" {
extern void func_ov002_020d71ec(void*, int);
int func_ov002_020d7030(char* a, char* b) {
  int flag = (*(int*)(b + 0xb0) & 0x60000) != 0;
  if (flag) return 0;
  *(int*)(((int)b + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x20000;
  *(void**)(b + 0xd0) = a;
  *(void**)(a + 0x360) = b;
  func_ov002_020d71ec(a, 1);
  return 1;
}
}
