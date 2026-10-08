extern unsigned short *data_0209d3b8;
unsigned int func_02018a24(unsigned int i){
  return i < 0x8000 ? data_0209d3b8[i] : i;
}
