extern signed char LEVEL_ID[];
extern unsigned char LEVEL_SPECIFIC_SETTING[];
extern int data_02075244[];
int _ZN5Actor17GetWaterHeightWDWEv(char *self) {
  int ret = *(int*)(self+0x60);
  if (LEVEL_ID[0] == 0x15 && *(signed char*)(self+0xcc) == 0) {
    unsigned int i = LEVEL_SPECIFIC_SETTING[0];
    if (i >= 3) i = 2;
    ret = data_02075244[i];
  }
  return ret;
}
