extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int data_ov019_021134a0[];
extern int data_ov019_02113468[];
int func_ov019_02111904(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, data_ov019_021134a0[1], 0, 0x1000, 0);
  *(int*)(c+0x130)=0x1000;
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char*)c+0x138, data_ov019_02113468[1], 0, 0x1000, 0);
  *(char*)(c+0x38f)=0;
  *(int*)(c+0x98)=0x5000;
  *(int*)(c+0x374)=4;
  return 1;
}
