extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int data_ov027_02113bf0[];
extern int func_ov030_02113be8[];
int func_ov018_021122ec(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, data_ov027_02113bf0[1], 0, 0x1000, 0);
  *(int*)(c+0x130)=0x1000;
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char*)c+0x138, func_ov030_02113be8[1], 0, 0x1000, 0);
  *(int*)(c+0x98)=0;
  *(int*)(c+0x374)=0;
  *(int*)(c+0x37c)=0;
  return 1;
}
