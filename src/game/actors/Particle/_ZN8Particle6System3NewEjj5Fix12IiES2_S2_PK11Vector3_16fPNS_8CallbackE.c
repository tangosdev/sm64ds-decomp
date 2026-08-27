struct Params
{
  int f20[8];
  int f3c_pad;
};
struct System;
extern struct System *_ZNK8Particle10SysTracker8Contents8FindDataEj(void *contents, unsigned int type);
extern void *func_02021a74(void *table, int u, void *args, const void *pos, void *cb);
extern void *data_0209ee74;
struct Vec3
{
  short x;
  short y;
  short z;
};
struct ParamBlk
{
  char pad20[0x20];
  int f20;
  int f24;
  int f28;
  char pad2c[0x3c - 0x2c];
  short f3c;
  short f3e;
  short f40;
};
typedef void (*VFn)(void *, void *);
struct VObj
{
  VFn *vtable;
};
struct System
{
  int f0;
  char pad4[6];
  unsigned char fa;
  char padb[1];
  struct ParamBlk *fc;
  struct VObj *f10;
};
struct Args3
{
  int a;
  int b;
  int c;
};
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int type, unsigned int u, int a, int b, int c, const struct Vec3 *pos, void *cb)
{
  struct System *new_var;
  void *contents;
  struct System *sys;
  struct Args3 args;
  args.a = a >> 3;
  contents = ((char *) data_0209ee74) + 8;
  args.b = b >> 3;
  args.c = (c >> 1) >> 2;
  new_var = _ZNK8Particle10SysTracker8Contents8FindDataEj(contents, type);
  sys = new_var;
  if (sys)
  {
    struct VObj *o;
    struct ParamBlk *p;
    sys->fa = 1;
    p = sys->fc;
    p->f20 = args.a;
    p->f24 = args.b;
    p->f28 = args.c;
    if (pos != 0)
    {
      p->f3c = pos->x;
      p->f3e = pos->y;
      p->f40 = pos->z;
    }
    o = sys->f10;
    if (o != 0)
    {
      o->vtable[0](o, p);
    }
    return sys->f0;
  }
  return (int) func_02021a74(contents, u, &args, pos, cb);
}
