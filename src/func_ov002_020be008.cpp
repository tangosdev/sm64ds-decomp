//cpp
extern "C" {
extern unsigned char CURRENT_GAMEMODE;
extern int _ZN6Player14ST_LEVEL_ENTERE;
extern signed char NEXT_HAT_CHARACTER;
extern int func_ov002_020e3078(char* self, void* s);
extern void _ZN6Player18SetNewHatCharacterEjjb(char* self, unsigned int a, unsigned int b, int c);
extern void _ZN6Player17SetNoControlStateEhih(char* self, unsigned char a, int b, unsigned char c);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
  unsigned int id, unsigned int a2, void* pos, void* rot, int a5, int a6);

void func_ov002_020be008(char* self){
  int b = (int)(CURRENT_GAMEMODE == 1);
  if (b != 0) return;
  if (func_ov002_020e3078(self, &_ZN6Player14ST_LEVEL_ENTERE) == 0) return;
  {
    int r5 = NEXT_HAT_CHARACTER;
    if (r5 < 0) return;
    if (r5 == *(int*)(self+8)) return;
    _ZN6Player18SetNewHatCharacterEjjb(self, r5 & 0xff, 0, 0);
    _ZN6Player17SetNoControlStateEhih(self, 8, -1, 0);
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
      0x10d, (r5 << 8) | 0xa, self+0x5c, 0, *(signed char*)(self+0xcc), -1);
    NEXT_HAT_CHARACTER = -1;
  }
}
}
