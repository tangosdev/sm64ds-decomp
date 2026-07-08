extern int SublevelToLevel(int i);
extern signed char LEVEL_ID;
int func_ov002_020e9630(void) {
  int lv = SublevelToLevel(LEVEL_ID);
  if (lv == 0xf || lv == 0x10 || lv == 0x11 || lv == 0x12 ||
      lv == 0x13 || lv == 0x14 || lv == 0x1d)
    return 1;
  return 0;
}
