//cpp
extern "C" {
int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*,int,int,int,int,unsigned);
int _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*,int,int,int,unsigned);
extern int data_ov066_0211ae74[];
extern int data_ov066_0211ae3c[];
extern int data_ov066_0211ae7c[];
extern int data_ov066_0211aebc[];
int func_ov066_021187c8(char* c){
  if(*(int*)(c+0x49c) == 2){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x360, data_ov066_0211ae74[1], 4, 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c+0x448, data_ov066_0211ae3c[1], 0x40000000, 0x1000, 0);
  } else {
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x360, data_ov066_0211ae7c[1], 4, 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c+0x448, data_ov066_0211aebc[1], 0x40000000, 0x1000, 0);
  }
  *(int*)(c+0x3bc) = 0;
  *(int*)(c+0x494) = 0;
  *(int*)(c+0x498) = 0;
  return 1;
}
}
