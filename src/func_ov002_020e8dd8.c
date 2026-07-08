
typedef unsigned char u8;
extern signed char LEVEL_ID;
extern unsigned char LAST_ENTRANCE_ID;
extern void _ZN9PowerStar13AddStarMarkerEv(void);
int func_ov002_020e8dd8(unsigned char *self)
{
  signed char g1 = LEVEL_ID;
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
      if (LAST_ENTRANCE_ID != 4)
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
