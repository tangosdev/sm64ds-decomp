extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern int func_ov075_0211addc(void* c, int x);
extern void func_ov075_0211abb0(void* c);
extern void func_ov075_0211ad60(void* c);
struct E { char pad[8]; int r8; int rc; char pad2[8]; };
void func_ov075_0211b1cc(char* c) {
  int i;
  if (DecIfAbove0_Short((unsigned short*)(c + 0xa2)) == 0) return;
  for (i = 0; i < *(unsigned short*)(c + 0xa4); i++) {
    struct E* e = (struct E*)(*(int*)(c + 0x80) + i * 0x18);
    e->r8 = func_ov075_0211addc(c, e->rc);
  }
  func_ov075_0211abb0(c);
  func_ov075_0211ad60(c);
  *(short*)(c + 0xa0) += 0x1111;
}
