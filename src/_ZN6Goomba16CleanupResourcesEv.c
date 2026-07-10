// NONMATCHING: different op / idiom (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void UnloadBlueCoinModel(void);
extern void _ZN13SharedFilePtr7ReleaseEv(void* p);
extern void UnloadSilverStarAndNumber(void);
extern void _ZN8CapEnemy14UnloadCapModelEv(char* c);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern char data_ov084_02130cf8[];
extern void* data_ov084_02130278[];

int _ZN6Goomba16CleanupResourcesEv(char* c) {
  if (*(int*)(c+0x460) == 2) {
    UnloadBlueCoinModel();
    _ZN13SharedFilePtr7ReleaseEv(data_ov084_02130cf8);
  }
  {
    int i = 0;
    do {
      _ZN13SharedFilePtr7ReleaseEv(data_ov084_02130278[i]);
      i = i + 1;
    } while (i < 7);
  }
  if ((unsigned char)(*(unsigned char*)(c+0x464) + 0xff) <= 1) {
    UnloadSilverStarAndNumber();
    _ZN8CapEnemy14UnloadCapModelEv(c);
  }
  if (*(int*)(c+0x460) == 3 && *(int*)(c+0x43c) != 0) {
    char* a = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c+0x43c));
    if (a != 0) {
      (*(unsigned char*)(a+0x602))--;
    }
  }
  return 1;
}
