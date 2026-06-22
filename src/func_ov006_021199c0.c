extern int func_ov004_020af68c(int a,int b,int c,int d,int e);
extern int data_ov006_02137ae8[];
void func_ov006_021199c0(char*c){
  if(*(unsigned char*)(c+0x5000+0x605)==0) return;
  int x=*(int*)(c+0x5000+0x5f8);
  int y=*(int*)(c+0x5000+0x5fc);
  func_ov004_020af68c(data_ov006_02137ae8[*(unsigned char*)(c+0x5000+0x606)],
    x>>0xc, y>>0xc, -1, -1);
}
