
extern void _ZN5Sound12PlayBank2_2DEj(unsigned);
void func_ov006_020dc6d0(int o, int idx)
{
  unsigned short *timer = (unsigned short *) ((((char *) o) + 0x519c) + (idx * 0x14));
  if ((*timer) != 0)
  {
    *timer = (*timer) - 1;
    if ((*timer) == 0x18)
    {
      _ZN5Sound12PlayBank2_2DEj(0xf1);
    }
    return;
  }
  *((unsigned char *) (((((char *) o) + (idx * 0x14)) + 0x5000) + 0x1a1)) = 1;
  *((unsigned short *) (((((char *) ((unsigned long long) o)) + (idx * 0x14)) + 0x5100) + 0x9e)) = 0;
}
