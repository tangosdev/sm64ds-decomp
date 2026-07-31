#include "types.h"
extern u8 data_ov007_0210342c[];
inline char *inline_fn(char *arg0)
{
  return *((char **) arg0);
}

int func_ov007_020b58e4(char *self)
{
  char *g = *((char **) data_ov007_0210342c);
  char *b = *((char **) ((*((char **) data_ov007_0210342c)) + 0xc));
  int flag = *((int *) (b + 0xc));
  char *a = inline_fn(self);
  int sel = *((short *) b);
  char *r3ptr = *((char **) (a + 0x24));
  int r7 = flag == 0;
  short *c = *((short **) (a + 4));
  int lr_v = *((u16 *) r3ptr);
  int ip_v = *((u16 *) a);
  int ret = c[1];
  switch (sel)
  {
    case 0:
      if (r7)
    {
      ret = 1;
    }
      break;

    case 1:
      if (r7)
    {
      int d = *((short *) (((char *) b) + 0xa));
      if (d == 0xe)
      {
        if (lr_v == 0)
        {
          ret = 3;
        }
        else
          if (lr_v == 7)
        {
          ret = 4;
        }
        else
          if (lr_v == 6)
        {
          ret = 5;
        }
      }
      else
        if ((lr_v == 0) || (lr_v == 6))
      {
        ret = 0;
      }
      else
      {
        ret = 1;
      }
    }
      break;

    case 3:
      if (r7)
    {
      if (lr_v == 0)
      {
        ret = 3;
      }
      else
        if (lr_v == 6)
      {
        *((int *) (((char *) a) + 0x10)) = 0;
        ret = 5;
      }
      else
        if (lr_v == 7)
      {
        ret = 4;
      }
    }
      break;

    case 2:
      if (r7)
    {
      if ((lr_v == 0) || (lr_v == 6))
      {
        ret = 4;
      }
    }
      break;

    case 7:
      if (r7)
    {
      if (lr_v == 2)
      {
        int e = *((int *) (((char *) (*((char **) data_ov007_0210342c))) + 0x20));
        if (((e == 1) && (ip_v == 4)) || ((e == 0) && ((ip_v == 3) || (ip_v == 5))))
        {
          ret = 3;
        }
        if (ip_v == 6)
        {
          int f = *((int *) (((char *) g) + 0xf0));
          ret = 3;
          if (f == 1)
          {
            *((short *) (((char *) (*((int **) (self + 4)))) + 0x70)) = 0;
          }
          else
          {
            *((short *) (((char *) (*((int **) (self + 4)))) + 0x70)) = 0x28;
          }
        }
      }
    }
      break;

    case 4:
      if (r7)
    {
      if (ip_v == 3)
      {
        ret = 0;
      }
      else
        if (ip_v == 5)
      {
        ret = 3;
      }
    }
      break;

    case 5:
      if (r7)
    {
      if (ip_v == 4)
      {
        ret = 0;
      }
      else
        if (ip_v == 5)
      {
        ret = 4;
      }
    }
      break;

    case 6:
      if (r7)
    {
      if (lr_v == 2)
      {
        ret = 4;
      }
    }
      break;

    case 14:
      if (r7)
    {
      if (lr_v == 0)
      {
        ret = 4;
      }
      else
        if (lr_v == 7)
      {
        ret = 3;
      }
    }
      break;

    case 8:
      if (r7)
    {
      int t = *((short *) (((char *) (*((int **) (((char *) (*((char **) data_ov007_0210342c))) + 8)))) + 0xa));
      if (t < 0)
      {
        if (lr_v == 3)
        {
          ret = 0;
        }
        else
        {
          ret = 1;
        }
      }
      else
        if (lr_v == 3)
      {
        ret = (ip_v != 0x15) ? (5) : (3);
      }
      else
        if (lr_v == 4)
      {
        ret = 4;
      }
    }
      break;

    case 9:
      if (r7)
    {
      if (((u32) (ip_v - 0xe)) <= 2)
      {
        ret = 6;
      }
      else
        if (ip_v == 0x15)
      {
        ret = 4;
      }
    }
    else
      if (flag == 1)
    {
      if (lr_v == 4)
      {
        int t = *((short *) (((char *) b) + 0xa));
        if (t == 8)
        {
          ret = 3;
        }
        else
        {
          *((int *) (((char *) a) + 0x10)) = 0;
          ret = 5;
        }
      }
    }
      break;

    case 10:

    case 12:

    case 13:
      if (r7)
    {
      if (lr_v == 4)
      {
        ret = 4;
      }
      else
        if (((u32) (ip_v - 0xe)) <= 2)
      {
        ret = 5;
      }
    }
      break;

    case 11:
      if (r7)
    {
      if (((u32) (ip_v - 0xe)) <= 2)
      {
        ret = 6;
      }
    }
      break;

  }

  return ret;
}
