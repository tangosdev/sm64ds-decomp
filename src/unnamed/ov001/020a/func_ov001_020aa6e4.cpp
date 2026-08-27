//cpp
extern "C" {
extern unsigned char data_ov001_020ad62c[];
extern unsigned char data_ov001_020ad624;
extern unsigned char data_0209f284;
extern unsigned char data_ov001_020ad620;
extern int func_02029600(int x);
extern void func_02012790(int n);
void func_ov001_020aa6e4(int idx, unsigned r1, int* obj){
  data_ov001_020ad62c[idx] |= 1;
  if((data_ov001_020ad62c[idx] & 0x80) && data_ov001_020ad624==0){
    if(obj==0) return;
    if(r1>=3) return;
    if(func_02029600(obj[1])==0) return;
    data_0209f284=1;
    func_02012790(0x24);
    *((char*)obj+0x1a)=0x3c;
    data_ov001_020ad620=*((unsigned char*)obj+0x1a);
    return;
  }
  data_ov001_020ad62c[idx] |= 0x80;
}
}
