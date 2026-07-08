extern unsigned char CURRENT_GAMEMODE;
extern unsigned char *_ZN5Actor13ClosestPlayerEv(unsigned char *t);
extern void func_02005ed8(unsigned char *t);
int _ZN8CapEnemy21DestroyIfCapNotNeededEv(unsigned char *t) {
  if (t[0x113] >= 6) return 1;
  if ((int)(CURRENT_GAMEMODE == 1) != 0) return 1;
  unsigned char *p = _ZN5Actor13ClosestPlayerEv(t);
  if (p == 0) { func_02005ed8(t); return 0; }
  if (t[0x113] != p[0x6d9]) return 1;
  func_02005ed8(t);
  return 0;
}
