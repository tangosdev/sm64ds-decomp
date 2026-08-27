struct T
{
  short f0;
  short f2;
  int f4;
  int f8;
  int fc;
};
struct S
{
  int f0;
  struct T *f4;
  struct T *f8;
};
extern struct S *data_ov007_02104ba0;
extern int func_ov007_020c92d0(struct T *p);
extern int func_ov007_020ba368(int t);
extern void func_ov007_020ba2e0(void);
extern int func_ov007_020ba05c(void);
extern void func_ov007_020b9f0c(void);
extern int func_ov007_020b9fc0(void);
extern void func_ov007_020b9ca8(void);
extern void func_ov007_020ba9bc(void);
void func_ov007_020b9640(void)
{
  struct T *p;
  short st;
  int fc;
  func_ov007_020c92d0(data_ov007_02104ba0->f4);
  p = data_ov007_02104ba0->f4;
  st = p->f0;
  fc = p->fc;
  switch (st)
  {
    case 1:
      if (func_ov007_020ba368(fc))
      {
        data_ov007_02104ba0->f4->f2 = 2;
      }
      break;

    case 2:
      if (fc == 0)
      {
        func_ov007_020ba2e0();
      }
      if (func_ov007_020ba05c())
      {
        data_ov007_02104ba0->f4->f2 = 3;
      }
      break;

    case 5:
      if (fc == 0)
      {
        func_ov007_020b9f0c();
        data_ov007_02104ba0->f8->f2 = 4;
      }
      else if (func_ov007_020b9fc0() != 0)
      {
        data_ov007_02104ba0->f4->f2 = 4;
        data_ov007_02104ba0->f8->f2 = 8;
      }

    case 4:
      func_ov007_020b9ca8();
      func_ov007_020ba9bc();
      break;
  }
}
