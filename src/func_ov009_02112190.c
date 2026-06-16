extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int data_ov009_02113eb8[];
extern int data_ov009_02113eb0[];
int func_ov009_02112190(char* c){
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov009_02113eb8), 1, -1);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, _ZN9Animation8LoadFileER13SharedFilePtr(data_ov009_02113eb0), 0, 0x1000, 0);
  return 1;
}
