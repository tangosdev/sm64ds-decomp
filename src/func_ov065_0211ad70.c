extern int RandomIntInternal(int* seed);
extern unsigned char LEVEL_SPECIFIC_SETTING;
extern short data_ov065_0211c0c8[];
extern int RNG_STATE;
extern signed char data_ov065_0211d26c[];
void func_ov065_0211ad70(char* c) {
  unsigned char idx;
  *(unsigned char*)(c + 0x336) = 0;
  *(int*)(c + 0x32c) = 0;
  *(int*)(c + 0x98) = 0;
  *(int*)(c + 0x5c) = *(int*)(c + 0x320);
  *(int*)(c + 0x60) = *(int*)(c + 0x324);
  *(int*)(c + 0x64) = *(int*)(c + 0x328);
  idx = LEVEL_SPECIFIC_SETTING;
  *(short*)(c + 0x334) = data_ov065_0211c0c8[idx];
  if (idx != 2) return;
  *(short*)(c + 0x334) = data_ov065_0211d26c[(unsigned short)((unsigned)RandomIntInternal(&RNG_STATE) >> 16) & 3];
}
