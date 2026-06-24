//cpp
extern "C" {
extern int _ZN9Animation8FinishedEv(void* a);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void* data_ov006_0213ae48;
void func_ov006_020c27c4(char* c){
  if (!_ZN9Animation8FinishedEv(c + 0x58)) return;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 8, *(void**)(c + 0x104), 0, 0, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0xc8, *(void**)(c + 0x144), 0, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, (void*)&data_ov006_0213ae48, 0, 0x800, 0);
}
}
