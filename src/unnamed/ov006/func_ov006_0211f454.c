
extern short data_02082214[];
void func_ov006_0211f454(char *c, int i)
{
  int n = i * 0x24;
  char *pm = c + 0x4668;
  char *pa = c + 0x466c;
  int *new_var;
  short *tbl = data_02082214;
  int angle = *((unsigned short *) (pa + n));
  int s = tbl[((angle >> 4) * 2) + 1];
  int mult = *((int *) (pm + n));
  char *p0 = c + 0x4660;
  *((int *) (p0 + n)) = (*((int *) (p0 + n))) + ((int) (((((long long) (*(new_var = &s))) * mult) + 0x800) >> 12));
  {
    int angle2 = *((unsigned short *) (pa + n));
    int mult2 = *((int *) (pm + n));
    int s2 = tbl[(angle2 >> 4) * 2];
    char *p1 = c + 0x4664;
    *((int *) (p1 + n)) = (*((int *) (p1 + n))) + ((int) (((((long long) s2) * mult2) + 0x800) >> 12));
  }
}
