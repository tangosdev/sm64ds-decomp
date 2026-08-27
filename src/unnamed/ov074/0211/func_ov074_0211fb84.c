extern int func_ov074_02120808();
void func_ov074_0211fb84(char* c){
  func_ov074_02120808(c);
  char* b=c+0x500;
  *(short*)(b+0xfa)=0;
  *(char*)(c+0x603)=0;
  unsigned short t=3-(unsigned char)c[0x604];
  if(t>2)t=2;
  *(int*)(c+0x3ec)=t<<12;
  *(char*)(c+0x607)=3;
}
