void func_ov006_020e95a4(char* c){
  int i;
  char* p = c;
  for (i = 0; i < 5; i++) {
    *(int*)(p+0x5208) = 0;
    *(int*)(p+0x520c) = 0;
    *(unsigned short*)(p+0x5214) = 0;
    *(unsigned char*)(p+0x5218) = 0;
    *(unsigned char*)(p+0x521a) = 0;
    *(unsigned char*)(p+0x521b) = 0;
    *(unsigned char*)(p+0x521c) = 0;
    p += 0x18;
  }
}
