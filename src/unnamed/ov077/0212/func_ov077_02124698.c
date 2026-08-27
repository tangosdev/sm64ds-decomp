extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int data_ov077_02127b28[];
extern int data_ov077_02127b20[];
int func_ov077_02124698(char* c){
  *(int*)(c+0x98)=0xc000;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, data_ov077_02127b28[1], 0, 0x1000, 0);
  *(int*)(c+0x130)=0x1000;
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char*)c+0x1b0, data_ov077_02127b20[1], 0, 0x1000, 0);
  *(int*)(c+0x1bc)=0x1000;
  *(unsigned char*)(c+0x41c)=0x96;
  *(int*)(c+0x3f4)=0;
  return 1;
}
