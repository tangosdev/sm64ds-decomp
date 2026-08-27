//cpp
extern "C" {
extern unsigned char data_0209f2d8;
extern int data_ov002_021104b4;
extern signed char data_02092114;
extern int func_ov002_020e3078(char* self, void* s);
extern void _ZN6Player18SetNewHatCharacterEjjb(char* self, unsigned int a, unsigned int b, int c);
extern void _ZN6Player17SetNoControlStateEhih(char* self, unsigned char a, int b, unsigned char c);
extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
  unsigned int id, unsigned int a2, void* pos, void* rot, int a5, int a6);

void func_ov002_020be008(char* self){
  int b = (int)(data_0209f2d8 == 1);
  if (b != 0) return;
  if (func_ov002_020e3078(self, &data_ov002_021104b4) == 0) return;
  {
    int r5 = data_02092114;
    if (r5 < 0) return;
    if (r5 == *(int*)(self+8)) return;
    _ZN6Player18SetNewHatCharacterEjjb(self, r5 & 0xff, 0, 0);
    _ZN6Player17SetNoControlStateEhih(self, 8, -1, 0);
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
      0x10d, (r5 << 8) | 0xa, self+0x5c, 0, *(signed char*)(self+0xcc), -1);
    data_02092114 = -1;
  }
}
}
