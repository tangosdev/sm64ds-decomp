void func_ov006_020e8c74(char* c, int i){
  int off = i * 0x14;
  char* b = c + 0x5288;
  int x = *(int*)(b + off);
  x = x - (x >> 6);
  *(int*)(b + off) = x;
  if(*(int*)(b + off) <= 0x900){
    *(unsigned char*)(c + off + 0x5000 + 0x291) = 2;
  }
}
