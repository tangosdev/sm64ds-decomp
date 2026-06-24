typedef unsigned short u16;
struct Vector3 { int x, y, z; };
extern char* _ZN5Actor13ClosestPlayerEv(void);
extern int Vec3_Dist(const struct Vector3* a, const struct Vector3* b);
extern void func_ov018_021123d0(char* c, int i);

void func_ov018_02111b3c(char* c) {
  char* p = _ZN5Actor13ClosestPlayerEv();
  char* r1;
  if (p == 0) return;
  if (Vec3_Dist((struct Vector3*)(c+0x364), (struct Vector3*)(p+0x5c)) > 0x5dc000) return;
  if (*(int*)(p+8) == 3) r1 = *(char**)(p+0x360);
  else r1 = *(char**)(p+0x358);
  if (r1 == 0) return;
  {
    int b = (int)(*(u16*)(r1+0xc) == 0x100);
    if (b == 0) return;
  }
  if (*(int*)(r1+8) != 0) return;
  *(char**)(c+0x374) = p;
  func_ov018_021123d0(c, 2);
}
