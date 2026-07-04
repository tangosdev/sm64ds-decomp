struct Vector3 { int x, y, z; };
extern char* _ZN5Actor13ClosestPlayerEv(void);
extern int Vec3_Dist(const struct Vector3* a, const struct Vector3* b);
extern short Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);

void func_ov084_0212f204(char* r4){
  struct Vector3 v;
  *(char**)(r4 + 0x460) = _ZN5Actor13ClosestPlayerEv();
  {
    char* p = *(char**)(r4 + 0x460);
    if (p != 0) {
      struct Vector3* pp = (struct Vector3*)(((int)p + 0x5c) & 0xFFFFFFFFFFFFFFFF);
      v.x = pp->x;
      v.y = pp->y;
      v.z = pp->z;
      *(int*)(r4 + 0x464) = Vec3_Dist((struct Vector3*)(r4 + 0x5c), &v);
      *(short*)(r4 + 0x468) = Vec3_HorzAngle((struct Vector3*)(r4 + 0x5c), &v);
      *(int*)(r4 + 0x46c) = *(unsigned char*)(*(char**)(r4 + 0x460) + 0x6de);
    } else {
      *(int*)(r4 + 0x464) = 0x7fffffff;
      *(short*)(r4 + 0x468) = *(short*)(r4 + 0x8e);
    }
  }
}
