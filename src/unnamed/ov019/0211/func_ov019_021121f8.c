extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int data_ov019_02113470[];
extern int data_ov019_02113468[];
int func_ov019_021121f8(char* c){
  *(int*)(c+0x9c)=0;
  *(int*)(c+0xa0)=0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, data_ov019_02113470[1], 0, 0x1000, 0);
  *(int*)(c+0x130)=0x1000;
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char*)c+0x138, data_ov019_02113468[1], 0, 0x1000, 0);
  *(int*)(c+0x374)=0;
  return 1;
}
