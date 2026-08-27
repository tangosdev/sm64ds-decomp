
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020e3b9c(char *p);
void func_ov006_020e668c(char *c)
{
  int i;
  char *r = c;
  for (i = 0; i < 0xb; i++)
  {
    *((int *) (r + 0x4660)) = 0;
    *((int *) (r + 0x4664)) = 0;
    *((int *) (r + 0x4668)) = 0;
    *((int *) (r + 0x466c)) = 0;
    *((int *) (r + 0x4670)) = 0;
    *((int *) (r + 0x467c)) = 0;
    *((short *) (r + 0x4682)) = 0;
    *((short *) (r + 0x4684)) = 0;
    *((short *) (r + 0x4686)) = 0;
    *((unsigned char *) (r + 0x4688)) = 0;
    *((unsigned char *) (r + 0x4689)) = 0;
    *((unsigned char *) (r + 0x468a)) = 0;
    r += 0x30;
  }

  for (i = 0; i < 5; i++)
  {
    char *q = c + (i * ((unsigned long) 0x10));
    *((int *) (q + 0x4870)) = 0;
    *((int *) (q + 0x4874)) = 0;
    *((short *) (q + 0x4878)) = 0;
    *((short *) (q + 0x487a)) = 0;
    *((unsigned char *) (q + 0x487c)) = 0;
    *((unsigned char *) (q + 0x487d)) = 0;
  }

  *((short *) (c + 0x55b4)) = 0;
  *((unsigned char *) (c + 0x55ba)) = 0;
  *((int *) (c + 0x5584)) = 0;
  *((int *) (c + 0x5588)) = 0;
  *((int *) (c + 0x5594)) = 0;
  *((int *) (c + 0x5598)) = 0;
  *((int *) (c + 0x559c)) = 0;
  *((short *) (c + 0x55b2)) = 0;
  *((unsigned char *) (c + 0x55b8)) = 0;
  *((unsigned char *) (c + 0x55b9)) = 0;
  *((unsigned char *) (c + 0x55bb)) = 0;
  *((unsigned char *) (c + 0x55bc)) = 0;
  *((short *) (c + 0x55b6)) = 0;
  *((short *) (c + 0x55b0)) = 0;
  *((unsigned char *) (c + 0x55bd)) = 0;
  func_ov004_020adb1c(0);
  func_ov006_020e3b9c(c);
}
