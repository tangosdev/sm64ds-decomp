
typedef unsigned short u16;
struct VramReg
{
  u16 w0;
  u16 pad[8];
  u16 f12;
};
extern struct VramReg gVramReg;
extern void Vram__Map(u16 bits);
void _ZN2GX15SetBankForSubBGEt(u16 x)
{
  int s = (int) x;
  int new_var;
  unsigned int notx = ~((unsigned int) x);
  u16 old = gVramReg.f12;
  new_var = notx & (gVramReg.w0 | old);
  gVramReg.f12 = x;
  gVramReg.w0 = new_var;
  if (s <= 0x80)
  {
    if (s >= 0x80)
    {
      goto sub248;
    }
    else
      if (((s <= 4) && (s >= 0)) && (s != 0))
    {
      if (s == 4)
      {
        *((volatile unsigned char *) 0x4000242) = 0x84;
      }
    }
  }
  else
    if (s == 0x180)
  {
    *((volatile unsigned char *) 0x4000249) = 0x81;
    goto sub248;
  }
  goto done;
  sub248:
  *((volatile unsigned char *) 0x4000248) = 0x81;

  done:
  Vram__Map(gVramReg.w0);

}
