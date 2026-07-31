
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
struct Vec3
{
  int x;
  int y;
  int z;
};
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm, u32 a);
extern void _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(void *c);
extern void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int d, int e, int f, void *v16, void *cb);
extern int _ZN5Actor13DistToCPlayerEv(void *c);
extern int _ZN5Actor18HorzAngleToCPlayerEv(void *c);
extern void _Z14ApproachLinearRsss(s16 *p, int a, int b);
extern void func_0201267c(int a, void *p);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
extern void *_ZNK12WithMeshClsn13GetWallResultEv(void *self);
extern int _ZNK10ClsnResult9GetClsnIDEv(void *self);
extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void func_02012694(int a, void *p);
extern void _ZN5Actor8PoofDustEv(void *c);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 a, int b, int d, int e);
extern void _ZN9ActorBase18MarkForDestructionEv(void *c);
void func_ov079_0212682c(char *c)
{
  struct Vec3 sum;
  u16 counter = *((u16 *) ((((int) c) + 0x100)));
  char *new_var;
  if (counter < 0x28)
  {
    *((s32 *) (c + 0x98)) = 0x3000;
    return;
  }
  if (counter < 0x32)
  {
    if (((int) counter) % 2)
    {
      *((s32 *) (c + 0x98)) = 0x3000;
    }
    else
    {
      *((s32 *) (c + 0x98)) = -0x3000;
    }
    return;
  }
  _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x150, 0);
  *((s32 *) (c + 0x98)) = -0x1e000;
  _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(c);
  Vec3_Add(&sum, (struct Vec3 *) (c + 0x5c), (struct Vec3 *) (c + 0xa4));
  *((void **) (c + 0x3d8)) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*((u32 *) (c + 0x3d8)), 0x90, sum.x, sum.y, sum.z, 0, 0);
  *((s32 *) (c + 0x98)) = 0x1e000;
  if (_ZN5Actor13DistToCPlayerEv(c) > 0x12c000)
  {
    _Z14ApproachLinearRsss((s16 *) (c + 0x94), _ZN5Actor18HorzAngleToCPlayerEv(c), 0x200);
  }
  *((s16 *) (((int) c) + 0x8e)) = *((s16 *) (c + 0x94));
  if ((*((u16 *) (c + 0x100))) == 0x32)
  {
    func_0201267c(0xd4, c + 0x74);
    *((s32 *) ((((int) c) + 0x128))) &= ~1;
  }
  {
    int hit = 0;
    if ((*((u16 *) (c + 0x100))) > 0x96)
    {
      hit = 1;
    }
    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x150) != 0)
    {
      void *wr = _ZNK12WithMeshClsn13GetWallResultEv(c + 0x150);
      if (_ZNK10ClsnResult9GetClsnIDEv(wr) != (-1))
      {
        if (_ZN5Actor10FindWithIDEj(_ZNK10ClsnResult9GetClsnIDEv(wr)) != (*((void **) (c + 0x3dc))))
        {
          hit = 1;
        }
      }
      else
      {
        hit = 1;
      }
    }
    if (hit == 0)
    {
      return;
    }
  }
  new_var = c + 0x100;
  if ((*((u16 *) new_var)) > 0x96)
  {
    func_02012694(0x79, c + 0x74);
    _ZN5Actor8PoofDustEv(c);
  }
  else
  {
    func_02012694(0x78, c + 0x74);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8f, sum.x, sum.y, sum.z);
  }
  _ZN9ActorBase18MarkForDestructionEv(c);
}
