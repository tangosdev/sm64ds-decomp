extern int data_ov006_0212ee94[];
void func_ov006_0211a714(char* c, int i) {
  int idx = i * 0x24;
  unsigned short v = *(unsigned short*)(c + 0x51c8 + idx);
  if (v != 0) {
    *(short*)(c + 0x51c8 + idx) = v - 1;
    if (*(short*)(c + 0x51c8 + idx) < 0)
      *(short*)(c + 0x51c8 + idx) = 0;
    return;
  }
  *(unsigned char*)(c + 0x51cd + idx) = 1;
  *(int*)(c + 0x51b0 + idx) += data_ov006_0212ee94[*(unsigned char*)(c + 0x51d2 + idx)];
  *(int*)(c + 0x51bc + idx) = -0x4000;
  *(unsigned char*)(c + 0x51d1 + idx) = 1;
}
