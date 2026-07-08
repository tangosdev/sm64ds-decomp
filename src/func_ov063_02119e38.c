typedef struct Vector3 { int x, y, z; } Vector3;

extern int func_ov063_0211a8a4(char *thiz);
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern void _Z14ApproachLinearRsss(short *dst, short target, short step);
extern int func_ov063_0211a0a8(char *thiz, int a1, int a2, int a3, int a4);
extern void func_ov063_0211a030(char *thiz, int a1, int a2);
extern void func_ov063_0211a964(char *thiz, int arg1);
extern void func_ov063_0211a960(char *thiz);

extern signed char LEVEL_ID;

void func_ov063_02119e38(char *thiz, int a1, short a2, int a3) {
  int thresh;
  short angle;

  if (func_ov063_0211a8a4(thiz) != 0) {
    if ((unsigned int)(*(unsigned short*)(thiz + 0x500 + 0xd4) << 0x17) >> 0x1f) {
    } else {
      if (*(unsigned short*)(thiz + 0x500 + 0xbe) != 0) {
        unsigned short *q = (unsigned short*)(((long long)(int)(thiz + 0x5be)) & 0xFFFFFFFFFFFFFFFFLL);
        *q = *q - 1;
      }
    }
    if (*(unsigned short*)(thiz + 0x500 + 0xc4) != 0) {
      unsigned short *q = (unsigned short*)(((long long)(int)(thiz + 0x5c4)) & 0xFFFFFFFFFFFFFFFFLL);
      *q = *q - 1;
    }

    {
      unsigned char st = *(unsigned char*)(thiz + 0x5cf);
      if (st == 0xf || st == 9)
        thresh = 0x7fffffff;
      else
        thresh = 0x5dc000;
    }

    if (LEVEL_ID == 3 &&
        Vec3_HorzDist((Vector3*)(thiz + 0x51c), (Vector3*)(*(char**)(thiz + 0x484) + 0x5c)) > thresh) {
      angle = Vec3_HorzAngle((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x51c));
    } else {
      if (Vec3_HorzDist((Vector3*)(thiz + 0x5c), (Vector3*)(*(char**)(thiz + 0x484) + 0x5c)) <= thresh)
        angle = *(short*)(thiz + 0x5b0);
      else
        angle = Vec3_HorzAngle((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x51c));
    }

    _Z14ApproachLinearRsss((short*)(thiz + 0x94), angle, a2);
    *(int*)(thiz + 0xa8) = 0;

    {
      char *p = *(char**)(thiz + 0x484);
      if (*(unsigned char*)(p + 0x6de) == 0) {
        int myY = *(int*)(thiz + 0x60);
        int otherY = *(int*)(p + 0x60);
        int dy = myY - otherY;
        if ((a1 << 0xc) < dy && dy < 0x1f4000 &&
            (*(int*)(thiz + 0x520) - myY) < 0xfa000) {
          *(int*)(thiz + 0xa8) = func_ov063_0211a0a8(thiz, myY, otherY, 0xa000, 0x2000);
        }
      }
    }

    func_ov063_0211a030(thiz, 0xa000 - *(int*)(thiz + 0x588), a3);
    if (*(int*)(thiz + 0x98) != 0) {
      func_ov063_0211a964(thiz, 0);
      return;
    }
    func_ov063_0211a960(thiz);
    return;
  }

  *(int*)(thiz + 0x98) = 0;
  *(int*)(thiz + 0xa8) = 0;
  func_ov063_0211a960(thiz);
}
