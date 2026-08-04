//cpp
extern "C" {
extern int func_ov006_020c2984(void*);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern int data_ov006_0213ae00;
void func_ov006_020c23a8(void* c){
  char* r4 = (char*)c;
  if (!func_ov006_020c2984(r4)) return;
  if (*(int*)(r4 + 0x68) != *(int*)(r4 + 0x124)) return;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(r4 + 8, *(void**)(r4 + 0x11c), 0, 0, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(r4 + 0xc8, *(void**)(r4 + 0x15c), 0, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &data_ov006_0213ae00, 0, 0x800, 0);
}
}
