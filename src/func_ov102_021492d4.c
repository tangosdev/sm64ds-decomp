typedef short s16;
typedef unsigned short u16;
struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };
extern void func_ov102_02149684(int* dst, int* src);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int id, unsigned int p, const struct Vector3* pos, const struct Vector3_16* rot, int a, int b);
extern char data_020a0edc[];
extern char CURRENT_GAMEMODE[];
extern char CAMERA[];

void func_ov102_021492d4(char* c) {
  struct Vector3 pos;
  struct Vector3_16 rot;
  func_ov102_02149684((int*)&pos, (int*)c);
  rot.x = *(u16*)(data_020a0edc);
  rot.y = *(u16*)(data_020a0edc+2);
  rot.z = *(u16*)(data_020a0edc+4);
  if ((int)(*(unsigned char*)(CURRENT_GAMEMODE) == 1) != 0) {
    rot.y = *(s16*)(c+0x8e);
  } else {
    rot.y = *(s16*)(*(char**)CAMERA + 0x17c) + 0x4000;
  }
  _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x159, 0, &pos, &rot, *(signed char*)(c+0xcc), -1);
}
