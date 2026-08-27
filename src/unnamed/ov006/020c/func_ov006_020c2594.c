
extern void func_02012174(int, int);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, void *, int, int, int, unsigned short);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern int data_ov006_0213ae90;
extern volatile long long data_ov006_0213adc0;
void func_ov006_020c2594(void *c)
{
  char *r4 = (char *) c;
  if ((*((int *) (r4 + 0x1a8))) == 0)
  {
    func_02012174(0, 3);
  }
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(r4 + 8, *((void **) (r4 + 0x13c)), 8, 0x40000000, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(r4 + 0xc8, *((void **) (r4 + 0x17c)), 0x40000000, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &data_ov006_0213ae90, 0x40000000, 0x800, 0);
  *((long long *) r4) = data_ov006_0213adc0;
}
