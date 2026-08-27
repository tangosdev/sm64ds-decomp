extern int func_ov005_020c007c(int c);
extern void func_02013598(int x);
extern unsigned char data_0209b300[];
void func_ov005_020c0030(int c, int r4){
  int r=func_ov005_020c007c(c);
  if(r==0) return;
  if(r4<8) func_02013598(r4+0x1c);
  else func_02013598(r4-8);
  data_0209b300[0]=1;
}
