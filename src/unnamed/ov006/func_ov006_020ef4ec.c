
void func_ov006_020c42bc(void);
void func_ov006_020c712c(void);
int _Z15ApproachLinear2Rsss(short *, short, short);
void func_ov006_020c7388(void);
void func_ov006_020c40e8(void);
struct G2
{
  int w[2];
};
extern struct G2 data_ov006_0213cc8c;
void func_ov006_020ef4ec(char *c)
{
  int *p = (int *) (((unsigned long long) (c + 0x5a64)) & 0xFFFFFFFFFFFFFFFFULL);
  char *b = c + 0x5000;
  *p = (*p) + ((*((int *) (b + 0xa68))) >> 12);
  if ((*((int *) ((c + 0x5000) + 0xa64))) > 0x1000)
  {
    *p = (*p) - 0x1000;
  }
  func_ov006_020c42bc();
  func_ov006_020c712c();
  if (_Z15ApproachLinear2Rsss((short *) (c + 0x5a74), 0, 1) == 0)
  {
    return;
  }
  func_ov006_020c7388();
  func_ov006_020c40e8();
  *((struct G2 *) (c + 0x5004)) = data_ov006_0213cc8c;
}
