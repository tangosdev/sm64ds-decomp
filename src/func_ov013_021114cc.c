extern int IsAreaShowing(int);
extern void func_ov013_02111430(char* t);
extern signed char NEXT_LEVEL_ID[];
extern short data_ov013_021116ac[];
extern unsigned char LEVEL_SPECIFIC_SETTING[];
int func_ov013_021114cc(char* c){
  if (NEXT_LEVEL_ID[0] <= 0) {
    short* p = (short*)(((int)c + 0x90) & 0xFFFFFFFFFFFFFFFF);
    *p = *p + data_ov013_021116ac[*(unsigned char*)(c+0x124)];
  } else if (IsAreaShowing(*(signed char*)(c+0xcc)) && *(unsigned char*)(c+0x124) == 0) {
    unsigned short d = (unsigned short)(-*(short*)(c+0x90));
    if (d < 0x2000) LEVEL_SPECIFIC_SETTING[0] = 3;
    else if (d < 0x6000 && d >= 0x2000) LEVEL_SPECIFIC_SETTING[0] = 0;
    else if (d < 0xa000 && d >= 0x6000) LEVEL_SPECIFIC_SETTING[0] = 2;
    else if (d < 0xe000 && d >= 0xa000) LEVEL_SPECIFIC_SETTING[0] = 1;
    else if (d >= 0xe000) LEVEL_SPECIFIC_SETTING[0] = 3;
  }
  func_ov013_02111430(c);
  return 1;
}
