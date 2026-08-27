extern char *data_ov007_02104bbc;
extern void func_ov007_020bde2c(int);
extern void func_0204f73c(char *c, int a);
extern void func_0204f7fc(char *c, int a, int b);
void func_ov007_020bda38(int v){
  char *r5 = data_ov007_02104bbc + 0xdc;
  func_ov007_020bde2c(0x37);
  int r4 = (v<<8)>>12;
  func_0204f73c(r5, r4+0x100);
  func_0204f7fc(r5, 0xf, r4);
}
