extern int func_ov004_020afdd0(int a,int b,int c,int d,int e);
extern int data_ov098_0213c4f0[];
void func_ov006_020e4b78(char*c){
  if(*(unsigned char*)(c+0x55b9)==0) return;
  int x=*(int*)(c+0x5584);
  int y=*(int*)(c+0x5588);
  func_ov004_020afdd0((int)data_ov098_0213c4f0,(x>>12)-0x20,(y>>12)-8,-1,0);
}
