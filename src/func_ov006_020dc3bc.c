extern int func_ov004_020afa20(int a,int b,int c,int d,int e);
extern int data_ov006_0213a9fc[];
void func_ov006_020dc3bc(char*c){
  if(*(unsigned char*)(c+0x5000+0x1a2)==0) return;
  int x=*(int*)(c+0x5000+0x194);
  int y=*(int*)(c+0x5000+0x198);
  func_ov004_020afa20(data_ov006_0213a9fc[*(unsigned char*)(c+0x5000+0x1a3)],
    x>>0xc, y>>0xc, -1, -1);
}
