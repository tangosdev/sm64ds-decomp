
extern int data_ov006_02140418;
extern int data_ov006_0213b018;
extern char *data_ov006_02140420;
extern void _ZN16dMgJump3DMario_c19func_ov006_020c81e0Ev(char *c);
extern void _ZN16dMgJump3DMario_c19func_ov006_020c76e0Ev(char *c);
void func_ov006_020c7418(void)
{
  int n = data_ov006_02140418;
  int i = 0;
  int off;
  data_ov006_0213b018 = 0;
  if (n <= 0)
  {
    return;
  }
  off = 0;
  do
  {
    _ZN16dMgJump3DMario_c19func_ov006_020c81e0Ev(data_ov006_02140420 + off);
    _ZN16dMgJump3DMario_c19func_ov006_020c76e0Ev(data_ov006_02140420 + off);
    i++;
    off += 0xb8;
  }
  while (i < data_ov006_02140418);
}
