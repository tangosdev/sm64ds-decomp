
typedef unsigned char u8;
typedef unsigned short u16;
extern void func_0205a61c(const void *src, void *dst, unsigned int size);
extern char *data_020a9db8;
u8 *func_02065c2c(u8 *src, u8 *dst)
{
  u8 *r4;
  dst[0] = src[0];
  switch (dst[0])
  {
    case 7:
      r4 = src + 2;
      dst[2] = src[1];
    {
      char *g = data_020a9db8;
      g = g + 0x1000;
      if (dst[2] > (*((int *) (g + 0x320))))
      {
        return 0;
      }
      func_0205a61c(r4, dst + 3, *((int *) (g + 0x31c)));
      r4 = r4 + (*((int *) ((data_020a9db8 + 0x1000) + 0x31c)));
    }
      break;

    case 8:
    {
      int lo = src[1];
      u16 *p = (u16 *) ((((int) dst) + 2) & 0xFFFFFFFFFFFFFFFFLL);
      r4 = src + 3;
      *((u16 *) (dst + 2)) = lo & 0xff;
      *((u16 *) ((((int) dst) + 2) & 0xFFFFFFFFFFFFFFFFLL)) = (*p) | ((src[2] << 8) & 0xff00);
    }
      break;

    case 9:
    {
      int lo = src[1];
      u16 *p = (u16 *) ((((int) dst) + 2) & 0xFFFFFFFFFFFFFFFFLL);
      *((u16 *) (dst + 2)) = lo & 0xff;
      {
        int hi = src[2];
        r4 = src + 3;
        *p = (*p) | ((hi << 8) & 0xff00);
      }
      func_0205a61c(r4, dst + 4, *((int *) ((data_020a9db8 + 0x1000) + 0x31c)));
      r4 = r4 + (*((int *) ((data_020a9db8 + 0x1000) + 0x31c)));
    }
      break;

    default:
      return 0;

  }

  return r4;
}
