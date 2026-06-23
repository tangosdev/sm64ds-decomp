
typedef unsigned char u8;
extern signed char data_0209f2f8;
extern unsigned char data_0209f264;
extern void _ZN9PowerStar13AddStarMarkerEv(void);
int func_ov002_020e8dd8(unsigned char *self)
{
  signed char g1 = data_0209f2f8;
  int t;
  if (g1 == 5)
  {
    if ((*((u8 *) (self + 0x49d))) == 5)
    {
      return;
    }
  }
  if (g1 == 0x16)
  {
    if ((*((u8 *) (self + 0x49d))) == 4)
    {
      if (data_0209f264 != 4)
      {
        return;
      }
      _ZN9PowerStar13AddStarMarkerEv();
      return;
    }
  }
  if ((*((u8 *) (self + 0x49a))) == 0)
  {
    t = *((int *) (self + 0x43c));
    if ((t != 2) && (t != 4))
    {
      return;
    }
  }
  _ZN9PowerStar13AddStarMarkerEv();
}
