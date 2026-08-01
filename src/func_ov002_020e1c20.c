typedef int Fix12i;
struct Vector3
{
  int x;
  int y;
  int z;
};

extern void _ZN6Player11ChangeStateERNS_5StateE(void *p, void *state);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *p, unsigned int a, int b, Fix12i c, unsigned int d);
extern int _ZNK6Player14GetBodyModelIDEjb(void *p, unsigned int a, int b);
extern int func_02012120(unsigned int p0, unsigned int p1, unsigned int p2, const struct Vector3 *p3, short p4);
extern int func_ov002_020bf27c(char *c, int r1);

extern unsigned char data_020a0e40;
extern unsigned char data_0209f49c[];
extern int data_ov002_021101b4;

void func_ov002_020e1c20(char *c)
{
  if ((*((unsigned char *) (c + 0x6e9))) & 0xc)
  {
    _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
    return;
  }

  if ((*((unsigned char *) (c + 0x6e3))) != 0)
  {
    if ((*((unsigned short *) (&data_0209f49c[data_020a0e40 * 0x18]))) & 2)
    {
      if ((*((unsigned char *) (c + 0x6e9))) & 4)
      {
        *((int *) (c + 0xa8)) = -0x20000;
      }
      *((int *) (c + 0x620)) = func_02012120(*((int *) (c + 0x620)), *((unsigned char *) (c + 0x6d9)), 0x18, (const struct Vector3 *) (c + 0x74), 0);
      *((int *) (c + 0x98)) = (int) (((((long long) (*((int *) (c + 0x98)))) * 0xf00) + 0x800) >> 12);
      if ((*((int *) (c + 0xa8))) < 0)
      {
        int *p = (int *) (((long long) (c + 0xa8)));
        *p = *p + 0xc00;
      }
      else
      {
        int *p = (int *) (((long long) (c + 0xa8)));
        *p = *p + 0x1000;
      }
      if ((*((int *) (c + 0xa8))) < 0x11000)
      {
        return;
      }
    }
    *((unsigned char *) (c + 0x6e3)) = 0;
    *((unsigned char *) (c + 0x6e5)) = 0;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x54, 0x40000000, 0x1000, 0);
    return;
  }

  *((int *) (c + 0x9c)) = func_ov002_020bf27c(c, -0x4000);
  if ((*((int *) (c + 0xa8))) >= 0)
  {
    *((int *) (c + 0x9c)) = -0x8000;
    if ((*((unsigned short *) (&data_0209f49c[data_020a0e40 * 0x18]))) & 2)
    {
      *((int *) (c + 0x9c)) = func_ov002_020bf27c(c, -0x3000);
    }
    return;
  }

  if ((*((int *) (c + 0xa8))) >= -0x8000)
  {
    return;
  }

  if ((*((unsigned short *) (&data_0209f49c[data_020a0e40 * 0x18]))) & 2)
  {
    if ((*((unsigned char *) (c + 0x6e5))) == 1)
    {
      *((unsigned char *) (c + 0x6e3)) = 1;
      *((int *) (c + 0x9c)) = 0;
      _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x3f, 0, 0x1000, 0);
      {
        int id;
        char *p = (char *) (((long long) ((int) ((*((char **) ((c + 0xdc) + (_ZNK6Player14GetBodyModelIDEjb(c, (*((int *) (c + 8))) & 0xff, 0) * 4)))) + 0x50))));
        *((int *) (p + 0xc)) = 0x4000;
      }
    }
  }
}
