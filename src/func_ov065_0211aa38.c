// NONMATCHING: different op / idiom (div=4). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.

extern short SINE_TABLE[];
void func_ov065_0211aa38(char *r0, char *r1)
{
  unsigned short ang = *((unsigned short *) (r0 + 0x8e));
  int idx = ang >> 4;
  int v = (*((int *) (r0 + 0x38c))) << 2;
  int c = SINE_TABLE[idx * 2];
  int res1 = (int) (((((long long) v) * c) + 0x800) >> 12);
  int *new_var;
  int *p = (int *) (r1 + 0x5c);
  *p = (*p) + res1;
  unsigned short ang2 = *((unsigned short *) (r0 + 0x8e));
  int idx2 = ang2 >> 4;
  int *q = p + 2;
  int v2 = (*((int *) (r0 + 0x38c))) << 2;
  int s = SINE_TABLE[(idx2 * 2) + 1];
  int res2 = (int) (((((long long) v2) * s) + 0x800) >> 12);
  *new_var = (*q) + res2;
}
