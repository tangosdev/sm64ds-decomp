unsigned func_02062734(char* base, unsigned mask, unsigned acc, int count){
  int i;
  unsigned short bit = 1;
  for (i = 0; i < count; i++){
    if (mask & bit) acc += *(unsigned short*)(base + 0x410);
    bit <<= 1;
  }
  return acc;
}
