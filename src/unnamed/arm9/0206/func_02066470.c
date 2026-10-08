extern unsigned data_020a9db8;
extern void func_02065d5c(unsigned char* buf, unsigned a);
extern void func_02067f2c(unsigned size, unsigned arg1, unsigned dest);

void func_02066470(unsigned char x, unsigned arg1){
  unsigned char buf[8];
  buf[0] = x;
  func_02065d5c(buf, data_020a9db8);
  func_02067f2c(6, arg1, data_020a9db8);
}
