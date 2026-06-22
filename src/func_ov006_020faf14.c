extern int func_ov004_020afdd0(int a,int b,int c,int d,int e);
extern int data_ov006_021375f4[];
void func_ov006_020faf14(char*c){
  if(*(unsigned char*)(c+0x5000+0xbc7)==0) return;
  int x=*(int*)(c+0x5000+0xbb0);
  int y=*(int*)(c+0x5000+0xbb4);
  func_ov004_020afdd0(data_ov006_021375f4[*(unsigned char*)(c+0x5000+0xbc9)],
    x>>0xc, y>>0xc, -1, -1);
}
