
extern void func_ov006_0211ddb8(char *p);
extern void func_ov006_0211d7d8(char *p);
extern void func_ov006_0211d688(char *p);
#pragma opt_strength_reduction off
void func_ov006_0211fbf8(char *p)
{
  int v;
  int i;
  char *new_var;
  char *q;
  v = 0;
  q = p;
  i = 0;
  do
  {
    *((int *) (q + 0x4660)) = v;
    *((int *) (q + 0x4664)) = v;
    *((short *) (q + 0x466e)) = v;
    *((short *) (q + 0x4670)) = v;
    *((short *) (q + 0x4672)) = v;
    *((char *) (q + 0x4677)) = v;
    *((char *) (q + 0x4678)) = v;
    *((char *) (q + 0x4679)) = v;
    new_var = q;
    *((char *) (new_var + 0x467a)) = v;
    *((char *) (q + 0x467b)) = v;
    *((char *) (q + 0x467c)) = v;
    *((char *) (q + 0x467d)) = v;
    *((char *) (q + 0x467e)) = v;
    i++;
    *((char *) (q + 0x4676)) = v;
    q += 0x24;
  }
  while (i < 0x10);
  q = p;
  v = 0;
  i = 0;
  do
  {
    *((int *) (q + 0x48a0)) = i;
    *((int *) (q + 0x48a4)) = i;
    *((char *) (q + 0x48a8)) = i;
    *((char *) (q + 0x48a9)) = i;
    v++;
    *((char *) (q + 0x48aa)) = i;
    q += 0xc;
  }
  while (v < 0x10);
  v = 0;
  do
  {
    *((char *) ((p + (v << 4)) + 0x496d)) = i;
    *((char *) ((p + (v << 4)) + 0x496e)) = i;
    v++;
  }
  while (v < 0x10);
  *((int *) (p + 0x4bec)) = i;
  *((int *) (p + 0x4bf0)) = i;
  *((int *) (p + 0x4bf4)) = i;
  *((int *) (p + 0x4bf8)) = i;
  *((int *) (p + 0x4bfc)) = i;
  *((int *) (p + 0x4c00)) = i;
  *((short *) (p + 0x4c10)) = i;
  *((short *) (p + 0x4c0c)) = i;
  *((short *) (p + 0x4c0e)) = i;
  *((char *) (p + 0x4c1d)) = i;
  *((char *) (p + 0x4c1f)) = i;
  *((char *) (p + 0x4c20)) = i;
  *((short *) (p + 0x4c14)) = i;
  *((char *) (p + 0x4c1a)) = i;
  *((char *) (p + 0x4c1b)) = i;
  *((char *) (p + 0x4c1c)) = i;
  *((char *) (p + 0x4c21)) = i;
  func_ov006_0211ddb8(p);
  func_ov006_0211d7d8(p);
  *((short *) (p + 0x4c18)) = i;
  *((char *) (p + 0x4c24)) = i;
  func_ov006_0211d688(p);
  *((int *) (p + 0x4c08)) = i;
  *((unsigned char *) (p + 0x4c26)) = 0xff;
  *((char *) (p + 0x4c27)) = i;
}
