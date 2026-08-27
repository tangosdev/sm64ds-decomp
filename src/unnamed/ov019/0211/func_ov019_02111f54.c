extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int,unsigned int,unsigned int,int,int);
extern int func_0201267c(int,void*);
extern int data_ov019_02113478[];
int func_ov019_02111f54(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0xd4, (void*)data_ov019_02113478[1], 0x40000000, 0x1000, 0);
  *(int*)((char*)c+0x9c) = -0x2000;
  *(int*)((char*)c+0xa0) = -0x3c000;
  *(int*)((char*)c+0xa8) = 0x16000;
  *(unsigned char*)((char*)c+0x38f) = 0;
  _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x14, 0x7f, 0x15666, 0);
  func_0201267c(0x138, (char*)c+0x74);
  *(int*)((char*)c+0x374) = 2;
  return 1;
}
