
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef signed char s8;
typedef unsigned int u32;
typedef int s32;
typedef struct 
{
  int x;
  int y;
  int z;
} Vector3;
typedef struct 
{
  s16 x;
  s16 y;
  s16 z;
} Vector3_16;
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 cc, const Vector3 *v, u32 e);
extern int RandomIntInternal(int *seed);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 f, const Vector3 *v, const Vector3_16 *r, int a, int b);
extern int func_ov060_02115a30(void *o);
extern void func_ov060_02111cc0(char *c, int idx, int a);
extern void func_02012694(int a, void *p);
struct TabEnt
{
  int a;
  int b;
};
extern struct TabEnt data_ov060_0211acd8;
extern struct TabEnt data_ov060_0211acf0;
extern s16 data_02082214[];
extern int data_0209e650;
void func_ov060_02114858(void *self)
{
  char *c = (char *) self;
  int new_var;
  int new_var2;
  s32 v134 = *((s32 *) (c + 0x134));
  if ((v134 == data_ov060_0211acd8.b) || ((v134 == data_ov060_0211acf0.b) && ((((u32) (((u32) (*((s32 *) (c + 0x12c)))) << 4)) >> 16) > 0x2a)))
  {
    s32 v12c = *((s32 *) (c + 0x12c));
    s32 v450 = *((volatile s32 *) (c + 0x450));
    s32 r4 = ((u32) (v12c << 4)) >> 16;
    if (v450 != 0x180)
    {
      *((s32 *) (c + 0x44c)) = 0;
    }
    *((s32 *) (c + 0x450)) = 0x180;
    new_var = 10;
    *((s32 *) (c + 0x44c)) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*((s32 *) (c + 0x44c)), 3, *((s32 *) (c + 0x450)), (const Vector3 *) (c + 0x74), 0);
    u16 t = *((u16 *) (c + 0x400));
    if ((t % 5) == 0)
    {
      s32 pos[3];
      pos[0] = *((s32 *) (c + 0x5c));
      pos[1] = *((s32 *) (c + 0x60));
      pos[2] = *((s32 *) (c + 0x64));
      {
        s32 y = pos[1];
        u16 ang = *((u16 *) (c + 0x94));
        s16 *tbl = data_02082214;
        s16 sx = tbl[(ang >> 4) * 2];
        new_var2 = (sx * 0xc8) + pos[0];
        pos[1] = y + 0xb4000;
        pos[0] = new_var2;
        ang = *((u16 *) (c + 0x94));
        pos[2] = (tbl[((ang >> 4) * 2) + 1] * 0xc8) + pos[2];
      }
      if ((r4 >= 0xf) && (r4 < 0x14))
      {
        s32 rnd = RandomIntInternal(&data_0209e650);
        u32 hi = ((u32) rnd) >> 16;
        u32 m = hi % 10u;
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x118, (m << new_var) | 0x11, (Vector3 *) pos, (const Vector3_16 *) (c + 0x92), *((s8 *) (c + 0xcc)), -1);
      }
      else
      {
        s32 rnd = RandomIntInternal(&data_0209e650);
        u32 hi = ((u32) rnd) >> 16;
        u32 m = hi % 10u;
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x118, (m << 10) | 1, (Vector3 *) pos, (const Vector3_16 *) (c + 0x92), *((s8 *) (c + 0xcc)), -1);
      }
    }
    {
      u16 *pt = (u16 *) ((((int) c) + 0x400) & 0xFFFFFFFFFFFFFFFFLL);
      *pt = (*pt) + 1;
    }
  }
  {
    int *p = (int *) ((((int) c) + 0x418) & 0xFFFFFFFFFFFFFFFFLL);
    *p |= 0x20000;
  }
  if (func_ov060_02115a30(c) == 0)
  {
    return;
  }
  new_var = 0x423;
  switch (*((u8 *) (c + new_var)))
  {
    case 0:
      func_ov060_02111cc0(c, 0x16, 0x40000000);
      func_02012694(0xb5, c + 0x74);
    {
      u8 *ps = (u8 *) ((((int) c) + 0x423) & 0xFFFFFFFFFFFFFFFFLL);
      *ps = (*ps) + 1;
    }
      return;

    case 1:
      func_ov060_02111cc0(c, 0x15, 0);
    {
      u8 *ps = (u8 *) ((((int) c) + 0x423) & 0xFFFFFFFFFFFFFFFFLL);
      *ps = (*ps) + 1;
    }
      return;

    case 2:
      func_ov060_02111cc0(c, 0x17, 0x40000000);
    {
      u8 *ps = (u8 *) ((((int) c) + 0x423) & 0xFFFFFFFFFFFFFFFFLL);
      *ps = (*ps) + 1;
    }
      return;

    case 3:
      *((s32 *) (c + 0x40c)) = 0;
    {
      int *p = (int *) (((long long) ((int) (c + 0x418))) & 0xFFFFFFFFFFFFFFFFLL);
      *p &= ~0x20000;
    }
      return;

  }

}
