
extern int data_ov006_02136e38;
int *func_ov004_020af770(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
inline char *inline_fn(int *arg0)
{
  return (char *) arg0;
}

void func_ov006_020e8b18(char *thiz)
{
  int *g;
  int i;
  for (i = 0; i < 3; i++)
  {
    if ((*((unsigned char *) ((thiz + 0x5000) + 0x292))) != 0)
    {
      int *o = func_ov004_020af770(*(&data_ov006_02136e38), (*((int *) ((thiz + 0x5000) + 0x280))) >> 12, (*((int *) ((thiz + 0x5000) + 0x284))) >> 12, -1, *((unsigned char *) ((thiz + 0x5000) + 0x293)), *((int *) ((thiz + 0x5000) + 0x288)), 0);
      if ((*((unsigned char *) ((thiz + 0x5000) + 0x291))) == 2)
      {
        int w0 = o[0];
        int w1 = o[1];
        o[0] = (w0 & (~0xc00)) | 0x400;
        *((unsigned short *) (((char *) o) + 4)) = ((*((unsigned short *) (inline_fn(o) + 4))) & (~0xf000)) | (((((unsigned) w1) << 0x10) >> 0x1c) << 12);
      }
    }
    thiz += 0x14;
  }

}
