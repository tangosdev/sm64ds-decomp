
typedef int Fix12i;
struct Vector3
{
  int x;
  int y;
  int z;
};
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *p, unsigned int a, int b, Fix12i c, unsigned int d);
extern int _ZNK6Player14GetBodyModelIDEjb(void *p, unsigned int a, int b);
extern int func_02012120(unsigned int p0, unsigned int p1, unsigned int p2, const struct Vector3 *p3, short p4);
extern int func_ov002_020bf27c(char *c, int r1);
extern int _ZN6Player6IsAnimEj(void *p, unsigned int a);
extern unsigned char data_020a0e40;
extern unsigned char data_0209f49c[];
void func_ov002_020e1e70(char *c)
{
  *((int *) (c + 0x9c)) = -0x4000;
  *((int *) (c + 0xa0)) = -0x4b000;
  if ((*((int *) (c + 0xa8))) >= 0)
  {
    *((int *) (c + 0x9c)) = -0x8000;
    if ((*((unsigned short *) (&data_0209f49c[data_020a0e40 * 0x18]))) & 2)
    {
      *((int *) (c + 0x9c)) = -0x3400;
    }
  }
  if ((*((int *) (c + 0xa8))) >= 0)
  {
    return;
  }
  if ((*((unsigned short *) (&data_0209f49c[data_020a0e40 * 0x18]))) & 2)
  {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x3f, 0, 0x1000, 0);
    {
      int id;
      char *p = (char *) (((long long) ((int) ((*((char **) ((c + 0xdc) + (_ZNK6Player14GetBodyModelIDEjb(c, (*((int *) (c + 8))) & 0xff, 0) * 4)))) + 0x50))));
      *((int *) (p + 0xc)) = 0x4000;
    }
    *((int *) (c + 0x620)) = func_02012120(*((int *) (c + 0x620)), *((unsigned char *) (c + 0x6d9)), 0x18, (const struct Vector3 *) (c + 0x74), 0);
    if ((*((unsigned char *) (c + 0x6ff))) != 0)
    {
      *((int *) (c + 0x9c)) = func_ov002_020bf27c(c, -0x4000) >> 1;
      *((int *) (c + 0xa0)) = -0x25800;
    }
    else
    {
      *((int *) (c + 0x9c)) = func_ov002_020bf27c(c, -0x4000);
    }
  }
  else
    if (_ZN6Player6IsAnimEj(c, 0x3f) != 0)
  {
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x54, 0, 0x1000, 0);
  }
}
