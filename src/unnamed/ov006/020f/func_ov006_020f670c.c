void* func_ov006_020f670c(char* c){
  int cnt = 0;
  int i = 0;
  char* p = c;
  for (; i < 0x14; ) {
    if (*(unsigned char*)(p + 0x51bb) != 0) {
      if (*(unsigned char*)(p + 0x51bc) != 1) {
        cnt++;
        break;
      }
    }
    i++;
    p += 0x18;
  }
  if (cnt != 0) return c;
  p = c;
  i = 0;
  {
    unsigned char v = 2;
    for (; i < 0x14; ) {
      *(unsigned char*)(p + 0x51bc) = v;
      i++;
      p += 0x18;
    }
  }
  {
    char* base = c + 0x5000;
    *(int*)(base + 0x3d8) = 0;
    *(int*)(base + 0x3d4) = 2;
    return base;
  }
}
