void func_ov006_0211a19c(char* c, int i) {
  int idx = i * 0x24;
  *(int*)(c + 0x51b0 + idx) += *(int*)(c + 0x51b8 + idx);
  *(int*)(c + 0x51b4 + idx) += *(int*)(c + 0x51bc + idx);
  *(int*)(c + 0x51bc + idx) += 0x200;
  if (*(int*)(c + 0x51bc + idx) <= 0) return;
  *(int*)(c + 0x51bc + idx) = 0;
  *(unsigned char*)(c + 0x51d1 + idx) = 2;
  *(short*)(c + 0x51c8 + idx) = 0;
}
