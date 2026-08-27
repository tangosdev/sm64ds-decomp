void func_ov006_0211ad94(char* c, int i){
  int n = i * 0x24;
  *(int*)(c + 0x51b0 + n) = *(int*)(c + 0x51b0 + n) + *(int*)(c + 0x5000 + n + 0x1b8);
  *(int*)(c + 0x51b4 + n) = *(int*)(c + 0x51b4 + n) + *(int*)(c + 0x51bc + n);
  *(int*)(c + 0x51bc + n) = *(int*)(c + 0x51bc + n) + 0x200;
  if (((*(int*)(c + 0x5000 + n + 0x1c0) - *(int*)(c + 0x51b4 + n)) >> 0xc) > 0x20)
    return;
  if (*(int*)(c + 0x51bc + n) > 0) {
    *(unsigned char*)(c + 0x5000 + n + 0x1cc) = 0;
    *(unsigned char*)(c + 0x5000 + n + 0x1cd) = 0;
  }
}
