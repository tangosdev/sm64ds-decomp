#pragma opt_common_subs off
extern void func_020731dc(void *a, void *b, void *node);
extern void NoOpDestructor(void);
extern int data_ov004_020bfe74;
extern int data_ov004_020bfe78;
extern int data_ov004_020bfe7c;
extern int data_ov004_020bfe80;
extern int data_ov004_020bfe84;
extern int data_ov004_020bfe88;
extern int data_ov004_020bfe8c;
extern int data_ov004_020bfe90;
extern int data_ov004_020bfe94;
extern int data_ov004_020bfe98;
extern int data_ov004_020bfe9c;
extern int data_ov004_020bfea0;
extern int data_ov004_020bfea4;
extern int data_ov004_020bfea8;
extern int data_ov004_020bfeac;
extern int data_ov004_020bfeb0;
extern int data_ov004_020bfeb4;
extern char data_ov004_020bfe20[];
extern char data_ov004_020bfdd8[];
extern char data_ov004_020bfdcc[];
extern char data_ov004_020bfde4[];
extern char data_ov004_020bfe68[];
extern char data_ov004_020bfe50[];
extern char data_ov004_020bfe44[];
extern char data_ov004_020bfe38[];
extern char data_ov004_020bfe2c[];
extern char data_ov004_020bfe14[];
extern char data_ov004_020bfe08[];
extern char data_ov004_020bfdb4[];
extern char data_ov004_020bfdf0[];
extern char data_ov004_020bfda8[];
extern char data_ov004_020bfe5c[];
extern char data_ov004_020bfdc0[];
extern char data_ov004_020bfdfc[];

void __sinit_ov004_020b9b24(void)
{
  short *p;
  p = (short *) (&data_ov004_020bfe74);
  ((short *) (&data_ov004_020bfe74))[0] = 0x20;
  ((short *) (&data_ov004_020bfe74))[1] = 0x10;
  func_020731dc((short *) (&data_ov004_020bfe74), (void *) NoOpDestructor, data_ov004_020bfe20);

  p = (short *) (&data_ov004_020bfe78); p[0] = 0x20; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfdd8);
  p = (short *) (&data_ov004_020bfe7c); p[0] = 0x18; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfdcc);
  p = (short *) (&data_ov004_020bfe80); p[0] = 0x1c; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfde4);
  p = (short *) (&data_ov004_020bfe84); p[0] = 0x1c; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfe68);

  /* size before addr via temps, stores via inline addr expr */
  {
    int sz = 0x30;
    int h = 0x10;
    ((short *) (&data_ov004_020bfe88))[0] = (short)sz;
    ((short *) (&data_ov004_020bfe88))[1] = (short)h;
    func_020731dc((short *) (&data_ov004_020bfe88), (void *) NoOpDestructor, data_ov004_020bfe50);
  }

  p = (short *) (&data_ov004_020bfe8c); p[0] = 0x18; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfe44);
  p = (short *) (&data_ov004_020bfe90); p[0] = 0x30; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfe38);
  p = (short *) (&data_ov004_020bfe94); p[0] = 0x30; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfe2c);
  p = (short *) (&data_ov004_020bfe98); p[0] = 0x30; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfe14);

  {
    int sz = 0x3c;
    int h = 0x10;
    ((short *) (&data_ov004_020bfe9c))[0] = (short)sz;
    ((short *) (&data_ov004_020bfe9c))[1] = (short)h;
    func_020731dc((short *) (&data_ov004_020bfe9c), (void *) NoOpDestructor, data_ov004_020bfe08);
  }

  p = (short *) (&data_ov004_020bfea0); p[0] = 0x1c; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfdb4);
  p = (short *) (&data_ov004_020bfea4); p[0] = 0x8; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfdf0);
  p = (short *) (&data_ov004_020bfea8); p[0] = 0x8; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfda8);
  p = (short *) (&data_ov004_020bfeac); p[0] = 0x18; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfe5c);
  p = (short *) (&data_ov004_020bfeb0); p[0] = 0x28; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfdc0);
  p = (short *) (&data_ov004_020bfeb4); p[0] = 0x20; p[1] = 0x10;
  func_020731dc(p, (void *) NoOpDestructor, data_ov004_020bfdfc);
}
