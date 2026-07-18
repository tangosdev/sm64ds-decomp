
typedef struct Vector3
{
  int x;
  int y;
  int z;
} Vector3;
extern int func_ov002_020c5d0c(char *c);
extern int func_ov002_020dec70(char *c);
extern int func_ov002_020bf40c(char *c);
extern void _ZN5Timer10ResetTimerEv(unsigned char *self);
extern void _ZN5Timer10StartTimerEv(unsigned char *self);
extern void _ZN5Timer9StopTimerEv(unsigned char *self);
extern unsigned long long _ZN5Timer7GetTimeEv(unsigned char *self);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, const Vector3 *, const void *, int, int);
extern int func_ov002_020d5338(char *c);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(char *self);
extern int func_ov002_020bfec0(char *self);
extern int func_ov002_020c0d90(char *c, int y);
extern int func_ov002_020d06c0(char *p);
extern int func_ov002_020c0cbc(char *c);
extern unsigned char data_ov002_02111184[];
extern unsigned char data_0209d4c8[];
extern signed char data_0209f2f8;
int func_ov002_020c0fb4(char *c)
{
  int r;
  switch (*((int *) (c + 0x664)))
  {
    case 4:

    case 5:
      if ((r = func_ov002_020c5d0c(c)) != 0)
    {
      return r;
    }
      break;

    case 19:
      if ((r = func_ov002_020dec70(c)) != 0)
    {
      return r;
    }
      if ((r = func_ov002_020c5d0c(c)) != 0)
    {
      return r;
    }
      break;

    case 18:
      if ((r = func_ov002_020bf40c(c)) != 0)
    {
      return r;
    }
      break;

    case 15:
      if (data_ov002_02111184[0] != 0)
    {
      break;
    }
      _ZN5Timer10ResetTimerEv(data_0209d4c8);
      data_ov002_02111184[0] = 1;
      _ZN5Timer10StartTimerEv(data_0209d4c8);
      break;

    case 16:
      if (data_0209d4c8[8] == 0)
    {
      break;
    }
      _ZN5Timer9StopTimerEv(data_0209d4c8);
    {
      unsigned long long t = _ZN5Timer7GetTimeEv(data_0209d4c8);
      if (((t << 6) / 0x1ff6210) > 0x14)
      {
        break;
      }
    }
    {
      Vector3 pos;
      int px;
      int py;
      int pz;
      py = *((int *) (c + 0x60));
      pz = *((int *) (c + 0x64));
      px = *((int *) (c + 0x5c));
      py = (py + 0x82000) & 0xFFFFFFFFFFFFFFFF;
      pos.x = px;
      pos.y = py;
      pos.z = pz;
      _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb2, 0x42, &pos, 0, *((signed char *) (c + 0xcc)), -1);
    }
      break;

  }

  if (data_0209f2f8 == 0x26)
  {
    if ((*((int *) (c + 0x60))) < (-0x3e8000))
    {
      *((int *) (c + 0x60)) = -0x3e8000;
      if ((r = func_ov002_020d5338(c)) != 0)
      {
        return r;
      }
    }
  }
  if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x380))
  {
    if ((*((int *) (c + 0x668))) == 1)
    {
      int d = (*((int *) (c + 0x60))) - (*((int *) (c + 0x648)));
      if (d < 0)
      {
        d = -d;
      }
      if (d < 0x2000)
      {
        if ((r = func_ov002_020d5338(c)) != 0)
        {
          return r;
        }
      }
    }
    if ((r = func_ov002_020bfec0(c)) != 0)
    {
      return r;
    }
  }
  if ((r = func_ov002_020c0d90(c, *((int *) (c + 0x64c)))) != 0)
  {
    return r;
  }
  if ((r = func_ov002_020d06c0(c)) != 0)
  {
    return r;
  }
  return func_ov002_020c0cbc(c);
}
