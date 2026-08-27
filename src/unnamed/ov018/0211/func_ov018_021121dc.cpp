//cpp
extern "C" {
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern int data_ov018_02113bf0[];
int func_ov018_021121dc(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov018_02113bf0[1], 0, 0x1000, 0);
  *(int*)(c+0x130)=0x1000;
  *(unsigned char*)(c+0x388)=0;
  *(unsigned char*)(c+0x389)=0x3c;
  *(int*)(c+0x37c)=1;
  return 1;
}
}
