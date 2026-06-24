typedef short s16;
extern int func_ov074_02121a20(void* c, int idx);
extern int func_ov074_021206c8(char* c);
extern void func_ov074_02121a4c(char* c, int i);
extern int _ZN5Actor13DistToCPlayerEv(char* c);
extern void func_ov074_021203e4(char* c, int i);
extern int _ZN5Actor18HorzAngleToCPlayerEv(char* c);
extern int AngleDiff(int a, int b);

void func_ov074_0211ffcc(char* c) {
  if (func_ov074_02121a20(c, 5) == 0) {
    if (func_ov074_021206c8(c) == 0) return;
    func_ov074_02121a4c(c, 5);
    return;
  }
  if (_ZN5Actor13DistToCPlayerEv(c) >= 0x4b0000) {
    func_ov074_021203e4(c, 3);
  } else {
    if (AngleDiff(_ZN5Actor18HorzAngleToCPlayerEv(c), *(s16*)(c+0x8e)) < 0x3000) {
      func_ov074_021203e4(c, 3);
      return;
    }
  }
  if (*(unsigned char*)(c+0x602) != 0) return;
  func_ov074_021203e4(c, 2);
}
