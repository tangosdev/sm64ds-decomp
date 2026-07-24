
typedef unsigned char u8;
typedef short s16;
struct OamAttr;
void _ZN3OAM9RenderSubEP7OamAttriiii(struct OamAttr *a, int b, int c, int d, int e);
int RandomIntInternal(int *seed);
extern int data_020a0db0;
extern struct OamAttr *data_ov002_0210be1c[];
extern int data_0209e650;
#pragma opt_strength_reduction off
void func_ov002_020f20f4(char *thiz)
{
  char *self = thiz;
  int i = 0;
  int zpos = 0;
  int zneg = 0;
  int two = 2;
  int m1 = -1;
  for (; i < 3; i++)
  {
    char *d = *((char **) (self + 0xd4));
    if ((*((u8 *) ((d + i) + 0x30))) != 0)
    {
      if ((data_020a0db0 & 1) == 0)
      {
        s16 *o = (s16 *) (d + ((i & 0xFFFFFFFFFFFFFFFF) * 2));
        s16 *q = (s16 *) (((int) (((char *) ((s16 *) (d + (i * 2)))) + 0xc)) & 0xFFFFFFFFFFFFFFFFLL);
        s16 vel = *((s16 *) (((char *) o) + 0x1c));
        s16 pos = *q;
        int *seed = &data_0209e650;
        *q = (s16) (pos + vel);
        RandomIntInternal(seed);
        d = *((char **) (self + 0xd4));
        int idx2 = ((unsigned long long) i) * 2;
        if ((*((s16 *) ((d + idx2) + 0x1c))) >= 0)
        {
          if ((*((s16 *) ((d + idx2) + 0xc))) >= 0x140)
          {
            *((u8 *) ((d + i) + 0x30)) = (u8) zpos;
            d = *((char **) (self + 0xd4));
            *((s16 *) ((d + idx2) + 0x24)) = (s16) ((i + 1) << 5);
          }
        }
        else
          if ((*((s16 *) ((d + idx2) + 0xc))) <= (-0x40))
        {
          *((u8 *) ((d + i) + 0x30)) = (u8) zneg;
          d = *((char **) (self + 0xd4));
          *((s16 *) ((d + idx2) + 0x24)) = (s16) ((i + 1) << 5);
        }
      }
      d = *((char **) (self + 0xd4));
      _ZN3OAM9RenderSubEP7OamAttriiii(data_ov002_0210be1c[i], *((s16 *) ((d + (i * 2)) + 0xc)), *((s16 *) ((d + (i * 2)) + 0x14)), m1, two);
    }
  }

}
