//cpp
extern "C" {
extern void func_ov004_020b1d60(int a0,int a1,int a2,int a3,int a4);
extern int data_ov004_020beb70;
extern int data_ov004_020bea14[];
extern int data_ov004_020beba8[];
void func_ov004_020adcc8(short* obj){
  if((data_ov004_020beb70 & 1) == 0){
    int* s = data_ov004_020bea14;
    int* d = data_ov004_020beba8;
    d[0]=s[0];d[1]=s[0];d[2]=s[0];d[3]=s[0];d[4]=s[0];
    d[5]=s[4];d[6]=s[4];d[7]=s[3];d[8]=s[2];d[9]=s[1];
    d[10]=s[2];d[11]=s[2];d[12]=s[3];d[13]=s[4];d[14]=s[4];d[15]=s[0];
    data_ov004_020beb70 |= 1;
  }
  {
    int idx=((int)*(short*)((char*)obj+0x1a))&0xf;
    int x=*(int*)obj>>0xc; int y=*(int*)((char*)obj+4)>>0xc;
    func_ov004_020b1d60(data_ov004_020beba8[idx],x,y,-1,-1);
  }
}
}
