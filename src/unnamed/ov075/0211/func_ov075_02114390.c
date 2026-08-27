extern int _ZNK9Animation12WillHitFrameEi(void*, int);
extern int func_02012174(int, int);
void func_ov075_02114390(char* r4) {
  if (!_ZNK9Animation12WillHitFrameEi(r4 + 0x50, 0x14)) return;
  if (*(unsigned char*)(r4 + 0x153) == 0) { *(unsigned char*)(r4 + 0x153) = 1; return; }
  func_02012174(*(unsigned char*)(r4 + 0x152), 0x1c);
  *(int*)(r4 + 0x110) = 0;
}
