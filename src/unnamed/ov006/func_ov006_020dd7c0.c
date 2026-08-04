extern int data_ov006_0212e400[];
void func_ov006_020dd7c0(char *thiz, int index)
{
  int off = index * 0x1c;
  int new_var;
  int *pa = (int *) ((thiz + 0x466c) + off);
  int *pb = (int *) ((thiz + 0x4664) + off);
  char *f = (thiz + off) + 0x4000;
  *pb = (*pb) + (*((int *) ((thiz + 0x466c) + off)));
  *((int *) ((thiz + 0x466c) + off)) = (*pa) + 0x400;
  new_var = 0x674;
  int look = data_ov006_0212e400[*((unsigned char *) (((thiz + off) - -0x4000) + new_var))];
  if (((*pb) >> 12) <= look)
  {
    return;
  }
  new_var = (*((unsigned char *) (((thiz + off) + 0x4000) + 0x67b))) * 0x18;
  char *g = (thiz + new_var) + 0x4000;
  *((int *) (((thiz + off) + 0x4000) + 0x660)) = *((int *) (g + 0xac0));
  *pb = look << 12;
  *pa = 0;
  *((int *) (((thiz + off) + 0x4000) + 0x668)) = 0;
  *((unsigned char *) (((thiz + off) + 0x4000) + 0x675)) = 4;
  pa += 0;
  *((unsigned char *) (((thiz + off) + 0x4000) + 0x676)) = 0;
}
