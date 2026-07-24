typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;
struct Vector3 { int x, y, z; };
extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern int Vec3_Dist(const void *a, const void *b);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const void *pos);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern int _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
extern int DecIfAbove0_Byte(u8 *p);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Actor13LandingDustAtER7Vector3b(void *self, struct Vector3 *v, int flag);
extern void func_0200d8c8(void *cam, const void *v, int strength);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern void func_ov016_02112a9c(void *self);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern s16 data_02082214[];
extern void *data_0209f318;
#pragma opt_common_subs off
inline int inline_fn(int arg0) { return arg0 & 0xFFFFFFFFFFFFFFFFLL; }

int func_ov016_02112b50(char *c)
{
  struct Vector3 pos;
  volatile struct Vector3 part;
  struct Vector3 dust;
  void *actor;
  int b;
  b = (int)((*(int *)(c + 0xb0) & 8) != 0);
  if (b != 0) return 1;
  actor = *(void **)(c + 0x324);
  if (actor != 0) {
    int *pp = (int *)inline_fn((int)actor + 0x5c);
    pos.x = pp[0]; pos.y = pp[1]; pos.z = pp[2];
  }
  switch (*(u8 *)(c + 0x31e)) {
  case 0:
    *(void **)(c + 0x324) = _ZN5Actor13ClosestPlayerEv(c);
    if (*(void **)(c + 0x324) != 0) {
      if (Vec3_Dist(c + 0x5c, &pos) < 0x514000) {
        u8 *p = (u8 *)inline_fn((int)c + 0x31e);
        *p = (u8)(*p + 1);
        *(u8 *)(c + 0x31f) = 0xa;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x56, c + 0x74);
      }
    }
    break;
  case 1:
    if (*(void **)(c + 0x324) != 0) {
      s16 ang = Vec3_HorzAngle(c + 0x5c, &pos);
      _Z14ApproachLinearRsss((s16 *)(c + 0x8e), ang, 0x400);
      if (DecIfAbove0_Byte((u8 *)inline_fn((int)c + 0x31f)) == 0) {
        u8 *p = (u8 *)inline_fn((int)c + 0x31e);
        *p = (u8)(*p + 1);
      }
    }
    break;
  case 2: {
    s16 *p322 = (s16 *)inline_fn((int)c + 0x322);
    s16 *p320 = (s16 *)inline_fn((unsigned int)c + 0x320);
    s16 *p8c = (s16 *)inline_fn((unsigned int)c + 0x8c);
    *p322 += 4;
    *p320 += ((s16 *)(c + 0x300))[0x11];
    *p8c += ((s16 *)(c + 0x300))[0x10];

    if (*(s16 *)(c + 0x8c) > 0x3900) {
      int x = *(int *)(c + 0x5c);
      s16 *tbl = data_02082214;
      part.x = x;
      int y = *(int *)(c + 0x60);
      part.y = y;
      int z = *(int *)(c + 0x64);
      int k = 0x384000;
      part.z = z;
      u16 a = *(u16 *)(c + 0x8e);
      int r = 0x800;
      y = y + 0x96000;
      s16 sn = tbl[(a >> 4) << 1];
      unsigned id = 0x107;
      x = x + (int)(((s64)sn * (s64)k + r) >> 12);
      part.x = x;
      a = *(u16 *)(c + 0x8e);
      s16 cs = tbl[((a >> 4) << 1) + 1];
      z = z + (int)(((s64)cs * (s64)k + r) >> 12);
      part.y = y;
      part.z = z;
      _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(id, x, y, z);
      dust.x = part.x; dust.y = part.y; dust.z = part.z;
      _ZN5Actor13LandingDustAtER7Vector3b(c, &dust, 1);
      func_0200d8c8(data_0209f318, c + 0x5c, 0x5dc000);
      _ZN5Sound9PlayBank3EjRK7Vector3(0x41, c + 0x74);
      _ZN9ActorBase18MarkForDestructionEv(c);
    }

    break;
  }
  }
  func_ov016_02112a9c(c);
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
  return 1;
}
