//cpp
extern "C" {
extern int func_ov006_020c2984(void*);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern int data_ov006_0213ae18;
void func_ov006_020c24e4(void* c){
  char* r4 = (char*)c;
  if (!func_ov006_020c2984(r4)) return;
  if (*(int*)(r4 + 0x68) != *(int*)(r4 + 0x13c)) return;
  if (*(int*)(r4 + 0x1a8) == 0) _ZN5Sound12PlayBank2_2DEj(0x1d1);
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(r4 + 8, *(void**)(r4 + 0x134), 8, 0, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(r4 + 0xc8, *(void**)(r4 + 0x174), 0, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &data_ov006_0213ae18, 0, 0x800, 0);
}
}
