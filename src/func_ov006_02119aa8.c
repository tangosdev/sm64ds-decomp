extern int func_ov004_020af68c(int a,int b,int c,int d,int e);
extern int data_ov006_02138d28[];
void func_ov006_02119aa8(char*c){
  if(*(unsigned char*)(c+0x5000+0x5f5)==0) return;
  int x=*(int*)(c+0x5000+0x5e8);
  int y=*(int*)(c+0x5000+0x5ec);
  func_ov004_020af68c(data_ov006_02138d28[*(unsigned char*)(c+0x5000+0x5f6)],
    x>>0xc, y>>0xc, -1, -1);
}
