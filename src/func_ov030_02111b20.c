typedef short s16;
struct Vector3 { int x, y, z; };
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, struct Vector3* v, unsigned int i);
extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern s16 Vec3_HorzAngle(const struct Vector3* a, const struct Vector3* b);
extern void _Z11UpdateAngleRssis(short* p, short a, int b, short c);
extern int _ZNK7PathPtr8NumNodesEv(void* self);

int func_ov030_02111b20(char* c) {
  struct Vector3 v;
  int d;
  s16 ang;
  int *p;
  int n;
  _ZNK7PathPtr7GetNodeER7Vector3j(c+0x398, &v, *(unsigned int*)(c+0x3a0));
  d = Vec3_HorzDist((struct Vector3*)(c+0x5c), &v);
  ang = Vec3_HorzAngle((struct Vector3*)(c+0x5c), &v);
  _Z11UpdateAngleRssis((short*)(c+0x8e), ang, 2, 0x400);
  *(s16*)(c+0x94) = *(s16*)(c+0x8e);
  if (d < *(int*)(c+0x98)) {
    n = _ZNK7PathPtr8NumNodesEv(c+0x398);
    p = (int*)((long long)(int)(c + 0x3a0) & 0xFFFFFFFFFFFFFFFFLL);
    n = n - 1;
    *p = *p + 1;
    if (*(int*)(c+0x3a0) >= n) return 1;
  }
  return 0;
}
