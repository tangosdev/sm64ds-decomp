// NONMATCHING: base materialization / addressing (div=4). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.

extern void *_ZN9ActorBasenwEj(unsigned);
extern volatile int _ZN9ActorBaseC1Ev(void *);
extern int _ZTV12ActorDerived[];
extern int _ZTV5Scene[];
extern int data_ov005_020c2490[];
int *func_ov005_020c21ec(void)
{
  unsigned char **new_var;
  int new_var5;
  int *p = (int *) _ZN9ActorBasenwEj(0xb0);
  int new_var4;
  int *new_var3;
  unsigned char **new_var2;
  if (!p)
  {
  }
  if (p)
  {
    unsigned char *f;
    new_var5 = (int) _ZTV12ActorDerived;
    f--;
    _ZN9ActorBaseC1Ev(p);
    p[0] = new_var5;
    p[0] = (int) _ZTV5Scene;
    f = ((unsigned char *) p) + 0x13;
    new_var3 = data_ov005_020c2490;
    f++;
    new_var2 = &f;
    *f |= 1;
    new_var = new_var2;
    *(*new_var) |= 4;
    new_var4 = 0;
    p[new_var4] = (int) new_var3;
  }
  return p;
}
