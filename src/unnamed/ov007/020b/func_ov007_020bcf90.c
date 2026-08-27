
typedef short s16;
struct Obj
{
  char pad0[4];
  s16 *m4;
  s16 *m8;
  char pad0c[0x28 - 0xc];
  s16 *m28;
  char pad2c[0x30 - 0x2c];
  int m30;
  int m34;
  int m38;
  int m3c;
  char pad40[0x44 - 0x40];
  int m44;
  char pad48[0x114 - 0x48];
  int m114[24];
};
extern struct Obj *data_ov007_0210342c;
extern void func_ov007_020c93b4(int a);
extern void func_020557b4(void);
extern void func_02055624(void);
extern void _ZN3G3X13SetClearColorEtiiib(int a, int b, int c, int d, int e);
extern void func_ov007_020b2160(int a);
extern void func_ov007_020b8fd4(int a);
extern void func_ov007_020b413c(int a, int b);
extern void func_ov007_020b2728(void);
extern int func_ov007_020b7a34(void);
extern void func_ov007_020b7a00(void);
extern void func_ov007_020b4464(int a, int b);
extern void func_ov007_020b2cf0(void);
extern void func_ov007_020c2390(int a);
extern void func_ov007_020c232c(int a);
extern void func_ov007_020b91b4(void);
extern void func_ov007_020bee14(void);
extern void func_ov007_020b7658(int a, int b);
extern void func_ov007_020b2370(void);
extern void func_ov007_020bfaf0(int a);
void func_ov007_020bcf90(void)
{
  s16 *pA;
  s16 *pB;
  int i;
  int j;
  int r4 = 1;
  pA = data_ov007_0210342c->m4;
  pB = data_ov007_0210342c->m8;
  if (((*pA) != 0) && (((*pA) != 1) || ((*pB) != 0)))
  {
    r4 = 0;
  }
  func_ov007_020c93b4(data_ov007_0210342c->m44);
  func_020557b4();
  func_02055624();
  _ZN3G3X13SetClearColorEtiiib(0x7fff, 0, 0x7fff, 0x3f, 1);
  func_ov007_020b2160(1);
  switch (*pA)
  {
    case 0:
      break;

    case 1:
      if ((*pB) == 2)
    {
      func_ov007_020b8fd4(data_ov007_0210342c->m34);
    }
      break;

    case 2:

    case 3:

    case 4:
      func_ov007_020b8fd4(data_ov007_0210342c->m34);
      break;

    case 5:
      for (i = 0xe; i <= 0x10; i++)
    {
      s16 *mp = &data_ov007_0210342c->m28[i - 0xe];
      func_ov007_020b413c(data_ov007_0210342c->m114[i], mp[2]);
    }

      break;

    case 6:
      break;

  }

  if (r4 != 0)
  {
    func_ov007_020b2728();
  }
  if (func_ov007_020b7a34() != 0)
  {
    func_ov007_020b7a00();
  }
  for (j = 0; j < 0x18; j++)
  {
    func_ov007_020b4464(data_ov007_0210342c->m114[j], data_ov007_0210342c->m34);
  }

  func_ov007_020b2cf0();
  func_ov007_020b2160(0);
  func_ov007_020c2390(data_ov007_0210342c->m38);
  func_ov007_020c232c(data_ov007_0210342c->m38);
  switch (*pA)
  {
    case 0:
      func_ov007_020c2390(data_ov007_0210342c->m3c);
      func_ov007_020c232c(data_ov007_0210342c->m3c);
      break;

    case 1:
      if ((*pB) == 2)
    {
      func_ov007_020b91b4();
    }
      break;

    case 2:

    case 3:

    case 4:
      func_ov007_020b91b4();
      break;

    case 5:
      func_ov007_020bee14();
      break;

    case 6:
      break;

  }

  func_ov007_020b7658(data_ov007_0210342c->m30 + 0x44, 1);
  if (r4 != 0)
  {
    func_ov007_020b2370();
  }
  func_ov007_020bfaf0(data_ov007_0210342c->m30);
}
