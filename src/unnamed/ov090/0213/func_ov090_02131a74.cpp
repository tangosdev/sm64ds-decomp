//cpp
extern "C" {
struct BCA_File; struct SharedPtr{void*a;BCA_File*f;};
extern struct SharedPtr data_ov090_02134480;
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,BCA_File*,int,int,unsigned);
int func_ov090_02131a74(char*c){
  *(int*)(c+0x3a4)=0x1000;
  *(short*)(c+0x396)=0;
  *(short*)(c+0x398)=0x32;
  *(int*)(c+0x390)=0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x30c, data_ov090_02134480.f, 0, 0x1000, 0);
  return 1;
}
}
