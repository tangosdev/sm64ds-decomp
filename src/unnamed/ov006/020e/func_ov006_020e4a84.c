
extern int func_ov004_020adbc0(void);
extern void func_ov004_020b1a5c(int, int);
extern void func_ov004_020b2444(int, int, int, int, int, int, int);
struct E22
{
  unsigned char b[0x10];
};
void func_ov006_020e4a84(unsigned char *c)
{
  struct E22 *a = (struct E22 *) c;
  int new_var;
  int i;
  func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
  for (i = 0; i < 5; i++)
  {
    unsigned char *p = (unsigned char *) (&a[(((((i & 0xFFFFFFFF) & 0xFFFFFFFF) & 0xFFFFFFFF) & 0xFFFFFFFF) & 0xFFFFFFFF) & 0xFFFFFFFF]);
    if (*((unsigned char *) (p + 0x487d)))
    {
      new_var = (*((int *) (p + 0x4870))) >> 12;
      func_ov004_020b2444(new_var, (*((int *) (p + 0x4874))) >> 12, *((unsigned short *) (p + 0x4878)), -1, -1, 0, 0);
    }
  }

}
