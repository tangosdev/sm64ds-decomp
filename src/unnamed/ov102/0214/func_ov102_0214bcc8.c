
extern void func_ov102_0214b3b8(void *c);
extern void func_ov102_0214c0b8(void *c);
extern void _ZN9Animation7AdvanceEv(void *a);
void func_ov102_0214bcc8(char *c)
{
  int r1 = *((int *) (c + 0xb0));
  int b = (int) ((r1 & 0x400) != 0);
  if (b)
  {
    func_ov102_0214b3b8(c);
  }
  else
  {
    int b2 = (int) ((r1 & 0x100) != 0);
    if (b2)
    {
    }
    else
    {
      func_ov102_0214c0b8(c);
    }
  }
  _ZN9Animation7AdvanceEv(c + 0x350);
}
