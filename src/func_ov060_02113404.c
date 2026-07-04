typedef short s16;
struct Vector3 { int x, y, z; };
extern int func_ov060_02112350(void* c);
extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern int _ZN5Actor14GetSubtractionEss(void* self, short a, short b);
extern char data_ov060_0211ac60[];

int func_ov060_02113404(char* c) {
  int r = 0;
  func_ov060_02112350(c);
  if (*(int*)(c+0x134) == *(int*)(data_ov060_0211ac60+4)) {
    char* base = *(char**)(c+0x3a0);
    if (base != 0) {
      int* o = (int*)(((int)base + 0x5c) & 0xFFFFFFFFFFFFFFFF);
      struct Vector3 v;
      s16 ang;
      v.x = o[0];
      v.y = o[1];
      v.z = o[2];
      ang = *(s16*)(*(char**)(c+0x3a0) + 0x8e);
      if (Vec3_HorzDist((struct Vector3*)(c+0x5c), &v) < 0x258000) {
        if (_ZN5Actor14GetSubtractionEss(c, ang, *(s16*)(c+0x406)) > 0x6000) r = 1;
      }
    }
  }
  *(s16*)(c+0x3fe) = 0;
  return r;
}
