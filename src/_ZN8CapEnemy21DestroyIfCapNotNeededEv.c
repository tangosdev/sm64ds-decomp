extern unsigned char data_0209f2d8;
extern unsigned char *_ZN5Actor13ClosestPlayerEv(unsigned char *t);
extern void func_02005ed8(unsigned char *t);
int _ZN8CapEnemy21DestroyIfCapNotNeededEv(unsigned char *t) {
  if (t[0x113] >= 6) return 1;
  if ((int)(data_0209f2d8 == 1) != 0) return 1;
  unsigned char *p = _ZN5Actor13ClosestPlayerEv(t);
  if (p == 0) { func_02005ed8(t); return 0; }
  if (t[0x113] != p[0x6d9]) return 1;
  func_02005ed8(t);
  return 0;
}
