
extern int data_ov006_0212e504[];
extern int data_ov006_0212e51c[];
struct E
{
  char p[0x30];
};
void func_ov006_020e6118(int this)
{
  int i;
  for (i = 0; i < 6; i++)
  {
    char *e = ((char *) (&((struct E *) this)[i + 5])) + 0x4000;
    *((unsigned char *) (e + 0x689)) = 1;
    *((unsigned char *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x68a)) = 1;
    *((int *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x660)) = data_ov006_0212e504[i] << 0xc;
    *((int *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x664)) = (-(data_ov006_0212e51c[i] + 0x20)) << 0xc;
    *((unsigned char *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x688)) = 2;
    *((int *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x668)) = 0;
    *((unsigned char *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x68d)) = 1;
  }

}
