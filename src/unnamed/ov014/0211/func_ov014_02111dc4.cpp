//cpp
extern "C" {
extern int data_ov014_02114980[];
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
void func_ov014_02111dc4(char *c){
  *(int*)(c+0xa8)=0;
  *(int*)(c+0x9c)=-0x2000;
  *(short*)(c+0x5fc)=0x78;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x150,(void*)data_ov014_02114980[1],0,0x1000,0);
}
}
