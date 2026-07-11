typedef struct { int x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;
typedef struct { short x, y; } SVec2;
extern int RandomIntInternal(int* seed);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, Vector3* v, Vector3_16* w, int e, int f);
extern int data_0209e650;

void func_ov100_02141848(char* c){
  Vector3_16 rot;
  int n;
  int i;
  n = (*(int*)(c+8) & 0xf) - 1;
  *(SVec2*)&rot = *(SVec2*)(c+0x92);
  rot.z = *(unsigned short*)(c+0x96);
  i = 0;
  if (n > 0) {
    do {
    Vector3 pos;
    int r;
    r = RandomIntInternal(&data_0209e650);
    pos.x = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + *(int*)(c+0x5c);
    r = RandomIntInternal(&data_0209e650);
    pos.y = *(int*)(c+0x60) + ((int)((unsigned int)r % 40) << 13);
    r = RandomIntInternal(&data_0209e650);
    pos.z = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + *(int*)(c+0x64);
    r = RandomIntInternal(&data_0209e650);
    rot.y = rot.y + ((r << 1) >> 16);
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x150, 0, &pos, &rot, *(signed char*)(c+0xcc), -1);
      i++;
    } while (i < n);
  }
  *(int*)(c+0x3e4) = 1;
}
