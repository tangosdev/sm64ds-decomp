// NONMATCHING: register allocation (div=6). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.

extern int func_0201267c(int, void *, int);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned);
extern void *_ZN5Actor13ClosestPlayerEv(void *);
extern short Vec3_VertAngle(const void *, const void *);
extern int data_ov014_02114970[];
extern short SINE_TABLE[];
void func_ov014_02111b70(char *c)
{
  int new_var;
  int new_var2;
  int new_var3;
  *((int *) (c + 0x9c)) = 0;
  func_0201267c(0x3a, ((char *) c) + 0x74, 0);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char *) c) + 0x150, (void *) data_ov014_02114970[1], 0, 0x1000, 0);
  char *p = (char *) _ZN5Actor13ClosestPlayerEv(c);
  int tgt[3];
  new_var3 = 0x5c;
  new_var = *((int *) (p + 0x64));
  new_var2 = *((int *) (p + new_var3));
  tgt[1] = (*((int *) (p + 0x60))) + 0x50000;
  tgt[0] = new_var2;
  tgt[2] = new_var;
  int a = (unsigned short) Vec3_VertAngle(tgt, (const void *) (c + 0x5c));
  int i = (a >> 4) << 1;
  *((int *) (c + 0xa8)) = -((int) (((((long long) SINE_TABLE[i]) * 0x8c000) + 0x800) >> 12));
  int v = *((int *) (c + 0xa8));
  if (v < 0x5000)
  {
    v = 0x5000;
  }
  else
    if (v > 0x2d000)
  {
    v = 0x2d000;
  }
  *((int *) (c + 0xa8)) = v;
  if (((!c) && (!c)) && (!c))
  {
  }
  *((int *) (c + 0x98)) = (int) (((((long long) SINE_TABLE[i + 1]) * 0x8c000) + 0x800) >> 12);
  *((short *) ((c + 0x500) + 0xfc)) = 0x3c;
}
