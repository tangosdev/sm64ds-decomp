typedef unsigned int u32;
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 p, const struct Vector3 *pos, const struct Vector3_16 *rot, int a, int b);
int func_ov064_02118760(char *c)
{
  int yadj;
  int zcopy;
  if (*(unsigned short *)(c + 0x100) == 0) {
    volatile int pos[3];
    *(unsigned short *)(c + 0x100) = 0xb4;
    int y = *(int *)(c + 0x60);
    int z = *(int *)(c + 0x64);
    yadj = 0x78000;
    yadj = y + yadj;
    zcopy = z;
    int x = *(int *)(c + 0x5c);
    pos[1] = yadj;
    pos[2] = zcopy;
    pos[0] = x;
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xd6, 0, (struct Vector3 *)(int *)pos, (struct Vector3_16 *)(c + 0x92), *(signed char *)(c + 0xcc), -1);
  }
  return 1;
}
