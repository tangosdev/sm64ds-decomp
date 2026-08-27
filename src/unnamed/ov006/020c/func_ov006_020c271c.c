
extern void func_02012174(int, int);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, void *, int, int, int, unsigned short);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern int data_ov006_0213aea8;
extern long long data_ov006_0213ade0;
inline char *inline_fn(char *arg0)
{
  return arg0 + 8;
}

void func_ov006_020c271c(void *c)
{
  char *r4 = (char *) c;
  unsigned long long new_var;
  long long *new_var2;
  if ((*((int *) (r4 + 0x1a8))) == 0)
  {
    func_02012174(0, 0x1c);
  }
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(inline_fn(r4), *((void **) (r4 + 0x114)), 4, 0x40000000, 0x800, 0);
  new_var2 = (long long *) r4;
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(r4 + 0xc8, *((void **) (r4 + 0x154)), 0x40000000, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &data_ov006_0213aea8, 0x40000000, 0x800, 0);
  new_var = data_ov006_0213ade0;
  *new_var2 = new_var;
}
