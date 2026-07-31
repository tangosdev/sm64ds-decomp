extern void func_ov006_020e6118(char *c);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int x);
extern void func_ov006_020e619c(char *c);
void func_ov006_020e5ffc(char *self)
{
  if ((*((unsigned char *) (self + 0x55bb))) == 0)
  {
    return;
  }
  if ((*((unsigned short *) ((self + 0x5500) + 0xb4))) != 0)
  {
    *((unsigned short *) (((int)self + 0x55b4) & 0xFFFFFFFFFFFFFFFF)) -= 1;
    if (*((short *) ((self + 0x5500) + 0xb4)) <= 0)
    {
      *((unsigned short *) ((self + 0x5500) + 0xb4)) = 0;
    }
    return;
  }
  *((unsigned char *) (self + 0x55bb)) = 0;
  {
    int n = *((unsigned char *) (self + 0x55ba));
    char *ip;
    char *p;
    char *q;
    if (n >= 5)
    {
      return;
    }
    ip = self + (n * 0x30);
    p = ip + 0x4000;
    *((unsigned char *) ((ip + 0x4000) + 0x689)) = 1;
    *((unsigned char *) ((ip + 0x4000) + 0x68a)) = 1;
    *((int *) (p + 0x660)) = 0x80000;
    *((int *) ((ip + 0x4000) + 0x664)) = 0x80000;
    q = ip + 0x4600;
    *((unsigned short *) (q + 0x80)) = 0;
    *((unsigned char *) ((ip + 0x4000) + 0x68b)) = 0;
    *((unsigned char *) ((ip + 0x4000) + 0x688)) = 0;
    *((unsigned char *) ((ip + 0x4000) + 0x68d)) = 0;
    *((unsigned char *) ((ip + 0x4000) + 0x68c)) = 0;
    if ((*((unsigned char *) (self + 0x55ba))) == 0)
    {
      func_ov006_020e6118(self);
    }
    else
    {
      _ZN5Sound12PlayBank2_2DEj(0x1d7);
    }
    *((unsigned char *) (((int)self + 0x55ba) & 0xFFFFFFFFFFFFFFFF)) += 1;
    *((int *) (self + 0x5584)) = 0x80000;
    *((int *) (self + 0x5588)) = 0xb0000;
    *((unsigned char *) (self + 0x55b8)) = 0;
    *((unsigned char *) (self + 0x55b9)) = 1;
    func_ov006_020e619c(self);
  }
}
