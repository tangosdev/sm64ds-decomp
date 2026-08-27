extern unsigned short data_020a0f1c;
int func_0203d7d4(int arg){
  int b;
  if (arg != 0) return 1;
  data_020a0f1c |= 0x80;
  b = (*(volatile unsigned short*)0x27ffc40 == 2);
  return b != 0;
}
