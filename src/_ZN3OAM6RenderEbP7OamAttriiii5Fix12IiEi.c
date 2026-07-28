
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef struct OamAttr
{
  u32 yb : 8;
  u32 rsFlags : 2;
  u32 objMode : 2;
  u32 mosaic : 1;
  u32 colorDepth : 1;
  u32 shape : 2;
  u32 xc : 9;
  u32 hi : 7;
  u32 tile : 10;
  u32 prio : 2;
  u32 pal : 4;
  u32 hi2 : 16;
} OamAttr;
typedef struct Matrix2x2
{
  s32 m00;
  s32 m01;
  s32 m10;
  s32 m11;
} Matrix2x2;
extern int _ZN3OAM11GetObjWidthEii(int shape, int size);
extern int _ZN3OAM12GetObjHeightEii(int shape, int size);
extern int _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(OamAttr *attr, int *count, Matrix2x2 *mtx);
extern u8 data_0209e660;
extern int data_0209e664;
extern int data_0209e668;
extern int data_0209e66c;
extern int data_0209e670;
extern OamAttr data_0209e674[];
extern OamAttr data_0209ea74[];
extern s16 data_02082214[];
OamAttr *_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int draw, OamAttr *obj, int px, int py, int pal, int prio, int scale0, int rot0)
{
  int *counter;
  int *affctr;
  int cnt;
  int h;
  int idx;
  int scale;
  int rot;
  int w;
  OamAttr *res;
  int x;
  int y;
  OamAttr *new_var;
  Matrix2x2 mtx;
  scale = scale0;
  rot = rot0;
  if ((!draw) || (data_0209e660 == 1))
  {
    counter = &data_0209e664;
    cnt = *counter;
    if (cnt >= 0x80)
    {
      return 0;
    }
    res = &data_0209e674[cnt];
    affctr = &data_0209e668;
  }
  else
  {
    counter = &data_0209e670;
    cnt = *counter;
    if (cnt >= 0x80)
    {
      return 0;
    }
    res = &data_0209ea74[cnt];
    affctr = &data_0209e66c;
  }
  x = obj->xc;
  if (x >= 0x100)
  {
    x -= 0x200;
  }
  y = *((s8 *) obj);
  w = _ZN3OAM11GetObjWidthEii(((*((u32 *) obj)) << 16) >> 30, (*((u32 *) obj)) >> 30);
  h = _ZN3OAM12GetObjHeightEii(((*((u32 *) obj)) << 16) >> 30, (*((u32 *) obj)) >> 30);
  if (rot != 0)
  {
    int sy;
    int sx;
    int c;
    int s;
    int i;
    i = (((u16) ((s16) rot)) >> 4) * 2;
    s = data_02082214[i];
    c = data_02082214[i + 1];
    sy = y + (h >> 1);
    sx = x + (w >> 1);
    x = (((sx * c) - (sy * s)) >> 12) - (w >> 1);
    y = (((sy * c) + (sx * s)) >> 12) - (h >> 1);
  }
  if ((scale != 0x1000) || (rot != 0))
  {
    if (obj->rsFlags != 1)
    {
      x -= w >> 1;
      y -= h >> 1;
      w <<= 1;
      h <<= 1;
    }
  }
  x += px;
  y += py;
  if (rot != 0)
  {
    if (w < h)
    {
      w = h;
    }
    if (((x + w) < 0) || (x > 0x100))
    {
      return 0;
    }
    if (((y + w) < 0) || (y > 0xc0))
    {
      return 0;
    }
  }
  else
  {
    if (((x + w) < 0) || (x > 0x100))
    {
      return 0;
    }
    if (((y + h) < 0) || (y > 0xc0))
    {
      return 0;
    }
  }
  if (pal == (-1))
  {
    pal = obj->pal;
  }
  if (prio == (-1))
  {
    prio = obj->prio;
  }
  {
    u32 flags;
    if ((scale != 0x1000) || (rot != 0))
    {
      int i = (rot >> 4) * 2;
      int s = data_02082214[i];
      int c = data_02082214[i + 1];
      int cs = ((c * scale) + 0x800) >> 12;
      int ss = ((s * scale) + 0x800) >> 12;
      mtx.m00 = cs;
      mtx.m10 = -ss;
      mtx.m11 = cs;
      mtx.m01 = ss;
      if ((*((u32 *) obj)) & 0x10000000)
      {
        mtx.m01 = -ss;
        mtx.m00 = -cs;
      }
      if ((*((u32 *) obj)) & 0x20000000)
      {
        mtx.m10 = -mtx.m10;
        mtx.m11 = -mtx.m11;
      }
      new_var = res - (*counter);
      idx = _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(new_var, affctr, &mtx);
      if (idx == (-1))
      {
        return 0;
      }
      if (obj->rsFlags == 1)
      {
        flags = 0x100;
      }
      else
      {
        flags = 0x300;
      }
    }
    else
    {
      idx = 0;
      flags = (*((u32 *) obj)) & 0x30000000;
    }
    {
      u32 mode;
      u32 mosaic;
      u32 mask;
      u32 dep;
      u32 tiles;
      tiles = obj->tile;
      {
        dep = obj->colorDepth;
        mask = (*((u32 *) obj)) & 0xc000c000;
        mosaic = obj->mosaic;
        mode = obj->objMode;
      }
      if ((flags == 0x100) || (flags == 0x300))
      {
        if (mode == 3)
        {
          *((u32 *) res) = flags | ((mask | ((((y & 0xff) | (idx << 25)) | (mode << 10)) | (mosaic << 12))) | ((x & 0x1ff) << 16));
        }
        else
        {
          *((u32 *) res) = flags | ((mask | ((((y & 0xff) | ((idx << 25) | (dep << 13))) | (mode << 10)) | (mosaic << 12))) | ((x & 0x1ff) << 16));
        }
      }
      else
        if (mode == 3)
      {
        *((u32 *) res) = flags | ((mask | (((y & 0xff) | (mode << 10)) | (mosaic << 12))) | ((x & 0x1ff) << 16));
      }
      else
      {
        *((u32 *) res) = flags | ((mask | ((((y & 0xff) | (dep << 13)) | (mode << 10)) | (mosaic << 12))) | ((x & 0x1ff) << 16));
      }
      *((u16 *) (((u32 *) res) + 1)) = (tiles | (prio << 10)) | (pal << 12);
    }
  }
  *counter += 1;
  return res;
}
