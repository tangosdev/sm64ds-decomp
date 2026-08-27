typedef struct { int x; } Fix12;
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj();
extern int data_ov090_02134488[];
int func_ov090_02131db0(char *c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x30c, data_ov090_02134488[1], 0x40000000, 0x1000, 0);
  *(int*)(c+0x3a4)=0x1000;
  *(int*)(c+0x390)=0;
  return 1;
}
