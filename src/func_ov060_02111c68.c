extern int *data_ov060_0211ac20[];
extern int *data_ov060_0211ac68[];
int func_ov060_02111c68(char *c){
  int v = *(int*)(c+0x134);
  if(v == (int)data_ov060_0211ac20[1]){
    unsigned int t = (*(unsigned int*)(c+0x12c) << 4) >> 0x10;
    if(t >= 0x31) return t - 0x31;
  }
  if(v == (int)data_ov060_0211ac68[1]){
    return ((*(unsigned int*)(c+0x12c) << 4) >> 0x10) + 0xb;
  }
  return -1;
}
