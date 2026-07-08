// NONMATCHING: different op / idiom (div=8). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef struct { int x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;
extern int RandomIntInternal(int* seed);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, Vector3* v, Vector3_16* w, int e, int f);
extern int RNG_STATE;

void func_ov100_02141848(char* c){
  Vector3_16 rot;
  int n = (*(int*)(c+8) & 0xf) - 1;
  rot.x = *(unsigned short*)(c+0x92);
  rot.y = *(unsigned short*)(c+0x94);
  rot.z = *(unsigned short*)(c+0x96);
  int i = 0;
  if (n > 0) {
    do {
    Vector3 pos;
    int r;
    r = RandomIntInternal(&RNG_STATE);
    pos.x = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + *(int*)(c+0x5c);
    r = RandomIntInternal(&RNG_STATE);
    pos.y = *(int*)(c+0x60) + ((int)((unsigned int)r % 40) << 13);
    r = RandomIntInternal(&RNG_STATE);
    pos.z = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + *(int*)(c+0x64);
    r = RandomIntInternal(&RNG_STATE);
    rot.y = rot.y + ((r << 1) >> 16);
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x150, 0, &pos, &rot, *(signed char*)(c+0xcc), -1);
      i++;
    } while (i < n);
  }
  *(int*)(c+0x3e4) = 1;
}
