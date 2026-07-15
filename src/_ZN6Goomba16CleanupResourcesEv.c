extern void UnloadBlueCoinModel(void* p);
extern void _ZN13SharedFilePtr7ReleaseEv(void* p);
extern void UnloadSilverStarAndNumber(void);
extern void _ZN8CapEnemy14UnloadCapModelEv(char* c);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern char data_ov084_02130cf8[];
extern void* data_ov084_02130278[];

int _ZN6Goomba16CleanupResourcesEv(char* c) {
  int i;
  if (*(int*)(c + 0x460) == 2)
    UnloadBlueCoinModel(c);
  _ZN13SharedFilePtr7ReleaseEv(data_ov084_02130cf8);
  for (i = 0; i < 7; ++i)
    _ZN13SharedFilePtr7ReleaseEv(data_ov084_02130278[i]);
  if ((unsigned char)(*(unsigned char*)(c + 0x464) + 0xff) <= 1)
    UnloadSilverStarAndNumber();
  _ZN8CapEnemy14UnloadCapModelEv(c);
  if (*(int*)(c + 0x460) == 3) {
    unsigned int id = *(unsigned int*)(c + 0x43c);
    if (id != 0) {
      char* a = _ZN5Actor10FindWithIDEj(id);
      if (a != 0) {
        unsigned char *p = (unsigned char*)(((int)a + 0x602) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p - 1;
      }
    }
  }
  return 1;
}
