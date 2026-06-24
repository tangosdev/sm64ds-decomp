extern void* data_ov006_02142504;
void func_ov006_020f84a8(char* o, int n) {
  *(short*)(o + 0x2a) = (short)n;
  if (n >= 0x14) {
    *(short*)(o + 0x28) = 1;
  } else {
    *(short*)(o + 0x28) = (short)((n % 5) * 2 + 1);
  }
  *(int*)(o + 0x24) = 0;
  if (n == 0x13) data_ov006_02142504 = o;
  *(unsigned char*)(o + 0x2e) = 1;
  *(int*)(o + 0x1c) = 0;
  *(int*)(o + 0x20) = 0;
  *(unsigned char*)(o + 0x2d) = 0;
  *(int*)(o + 8) = 0;
  *(int*)(o + 4) = *(int*)(o + 8);
}
