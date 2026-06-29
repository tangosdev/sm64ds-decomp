typedef unsigned char u8;
struct S
{
  char _0[0xe8];
  int f_e8;
  int f_ec;
  char _f0[4];
  int f_f4;
  char _f8[8];
  int f_100;
  int f_104;
  int f_108[3];
};
extern struct S *data_ov007_0210342c;
extern int data_ov007_020ccc48[];
extern int data_ov007_020ccc80[][3];
extern int data_ov007_020ccc68[][3];
extern void func_ov007_020b3edc(int r0);
extern void func_ov007_020bc53c(int r0);
extern void func_ov007_020bea44(int r0);
extern void func_ov007_020c44c4(int r0);
void func_ov007_020b283c(void)
{
  struct S *p;
  int ec;
  unsigned long long idx;
  int b;
  int i;
  p = data_ov007_0210342c;
  if (p->f_e8 == 0)
  {
    return;
  }
  ec = p->f_ec;
  idx = ec;
  b = *((u8 *) ((((char *) p) + ec) + 0xfc));
  if ((ec == 0) && (b == 0))
  {
    idx = 2;
  }
  func_ov007_020b3edc(data_ov007_020ccc48[ec]);
  for (i = 0; i < 3; i++)
  {
    func_ov007_020b3edc(data_ov007_020ccc80[idx][i]);
    func_ov007_020b3edc(data_ov007_020ccc68[idx][i]);
  }

  if (data_ov007_0210342c->f_f4 != 0)
  {
    func_ov007_020bc53c(data_ov007_0210342c->f_f4);
    data_ov007_0210342c->f_f4 = 0;
  }
  if (data_ov007_0210342c->f_100 != 0)
  {
    func_ov007_020bea44(data_ov007_0210342c->f_100);
    data_ov007_0210342c->f_100 = 0;
  }
  if (data_ov007_0210342c->f_104 != 0)
  {
    func_ov007_020bea44(data_ov007_0210342c->f_104);
    data_ov007_0210342c->f_104 = 0;
  }
  for (i = 0; i < 3; i++)
  {
    if (data_ov007_0210342c->f_108[i] != 0)
    {
      func_ov007_020c44c4(data_ov007_0210342c->f_108[i]);
      data_ov007_0210342c->f_108[i] = 0;
    }
  }

  data_ov007_0210342c->f_e8 = 0;
}
