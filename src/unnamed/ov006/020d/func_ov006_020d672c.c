extern int func_ov004_020b2444(int,int,int,int,int,int,int);
void func_ov006_020d672c(int c){
  if(*(unsigned char*)(c+0x6000+0x2f9)==0) return;
  func_ov004_020b2444(0x80,0xc,*(unsigned short*)(c+0x6200+0xee),1,-1,0,0);
}
