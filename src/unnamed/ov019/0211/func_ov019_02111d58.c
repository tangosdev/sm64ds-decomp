extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int data_ov019_02113480[];
extern int data_ov019_021134b0[];
int func_ov019_02111d58(char* c){
  *(int*)(c+0x9c)=-0x2000;
  *(int*)(c+0xa0)=-0x3c000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, data_ov019_02113480[1], 0x40000000, 0x1000, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char*)c+0x138, data_ov019_021134b0[1], 0, 0x1000, 0);
  *(int*)(c+0x98)=0x30000;
  *(char*)(c+0x38f)=0;
  *(char*)(c+0x393)=0;
  *(int*)(c+0x374)=3;
  return 1;
}
