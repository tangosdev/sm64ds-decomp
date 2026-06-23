extern int data_ov006_02135fc8[];
extern int data_ov006_021350fc[];
extern void func_ov004_020afa20(int,int,int,int,int);
void func_ov006_0211cca8(unsigned char* c){
  int idx,v,p,q;
  if(*(unsigned char*)(c+0x4be2)==0) return;
  idx=*(unsigned char*)(c+0x4be3);
  p=*(int*)(c+0x4bcc)>>12;
  q=*(int*)(c+0x4bd0)>>12;
  if(*(unsigned char*)(c+0x4be4)!=0) idx+=4;
  if(*(unsigned char*)(c+0x4be1)==8) v=data_ov006_02135fc8[idx];
  else v=data_ov006_021350fc[idx];
  func_ov004_020afa20(v,p,q,-1,-1);
}
