//cpp
extern "C" {
extern void _ZN5Sound12PlayBank2_2DEj(unsigned);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
struct G2 { int w[2]; };
extern struct G2 data_ov006_0213adf8;
extern void* data_ov006_0213ae30;
void func_ov006_020c2440(char* c){
  if (*(int*)(c + 0x1a8) == 0) _ZN5Sound12PlayBank2_2DEj(0x1c9);
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 8, *(void**)(c + 0x124), 0, 0x40000000, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0xc8, *(void**)(c + 0x164), 0x40000000, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, (void*)&data_ov006_0213ae30, 0x40000000, 0x800, 0);
  *(struct G2*)c = data_ov006_0213adf8;
}
}
