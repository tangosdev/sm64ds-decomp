
extern int data_ov006_02140418;
extern int data_ov006_0213b018;
extern char *data_ov006_02140420;
extern void func_ov006_020c81e0(char *c);
extern void func_ov006_020c76e0(char *c);
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
    func_ov006_020c81e0(data_ov006_02140420 + off);
    func_ov006_020c76e0(data_ov006_02140420 + off);
    i++;
    off += 0xb8;
  }
  while (i < data_ov006_02140418);
}
