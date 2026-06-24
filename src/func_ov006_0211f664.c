void func_ov006_0211f664(char* c, int i) {
  int idx = i * 0x24;
  if (*(unsigned char*)(c + 0x4678 + idx) >= 6) return;
  *(short*)(c + 0x4670 + idx) = *(unsigned short*)(c + 0x4670 + idx) + 1;
  if (*(unsigned short*)(c + 0x4670 + idx) < 0xf) return;
  *(short*)(c + 0x4670 + idx) = 0;
  *(unsigned char*)(c + 0x467d + idx) = *(unsigned char*)(c + 0x467d + idx) + 1;
  if (*(unsigned char*)(c + 0x467d + idx) >= 3)
    *(unsigned char*)(c + 0x467d + idx) = 0;
}
