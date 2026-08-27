extern unsigned char data_ov066_0211ae0c[];
int func_ov066_021168b0(char* c){
  if(*(int*)(c+0x4a0)==0){
    if(data_ov066_0211ae0c[0]!=3) return 0;
    *(int*)(c+0x4a0)=1;
    data_ov066_0211ae0c[0]=0;
  }
  return 1;
}
