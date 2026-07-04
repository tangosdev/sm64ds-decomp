typedef int Fix12;
typedef short s16;
extern void func_020393a4(int* p, int v);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov022_02111d48(char* t);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char* c, Fix12 a, Fix12 b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char* c);

int func_ov022_02111dfc(char* c) {
  func_020393a4((int*)(c+0x124), 0x650000);
  if (DecIfAbove0_Byte((unsigned char*)(c+0x320)) == 0) {
    int r1;
    s16* a = (s16*)(((int)c + 0x8c) & 0xFFFFFFFFFFFFFFFF);
    *a += *(s16*)(c+0x31e);
    r1 = *(s16*)(c+0x8c);
    if (r1 >= 0x400 || r1 <= -0x400) {
      *(s16*)(c+0x31e) = -*(s16*)(c+0x31e);
      *(unsigned char*)(c+0x320) = 0x1e;
    }
  }
  func_ov022_02111d48(c);
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0) {
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
  }
  return 1;
}
