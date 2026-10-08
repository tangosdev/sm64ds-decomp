extern int func_ov007_020c02d8();
extern int func_ov007_020c1d78();
extern int* data_ov007_0210342c;
int func_ov007_020adb84(void){
  int r4 = *(int*)(data_ov007_0210342c + 0xdc/4);
  func_ov007_020c02d8(*(int*)(*(int*)(r4+4)+0x10), 0x18, 0x300);
  int r1 = *(int*)(r4+4);
  *(int*)(r1+0xc) = 1;
  return func_ov007_020c1d78(0);
}
