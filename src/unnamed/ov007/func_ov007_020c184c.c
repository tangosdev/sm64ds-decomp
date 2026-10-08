extern int func_02053ed0(void);
extern int func_02053ec0(void);

extern int data_ov007_02104bdc;
extern int data_ov007_02104be4;
extern int data_ov007_02104be8;
extern int data_ov007_02104be0;

void func_ov007_020c184c(void) {
  int a = func_02053ed0();
  int b = func_02053ec0();

  data_ov007_02104bdc = 0;
  data_ov007_02104be4 = 0;
  data_ov007_02104be8 = 0;
  data_ov007_02104be0 = 0;

  switch (a) {
  case 1:
  case 2:
  case 4:
  case 8:
    data_ov007_02104be4 = 0x20000;
    break;
  case 3:
  case 5:
  case 6:
  case 9:
  case 10:
  case 12:
    data_ov007_02104be4 = 0x40000;
    break;
  case 7:
  case 11:
  case 13:
  case 14:
    data_ov007_02104be4 = 0x60000;
    break;
  case 15:
    data_ov007_02104be4 = 0x80000;
    break;
  }

  switch (b) {
  case 0:
    break;
  case 0x20:
  case 0x40:
    data_ov007_02104be0 = 0x4000;
    break;
  case 0x60:
    data_ov007_02104be0 = 0x8000;
    break;
  case 0x10:
    data_ov007_02104be0 = 0x10000;
    break;
  case 0x30:
    data_ov007_02104be0 = 0x14000;
    break;
  case 0x70:
    data_ov007_02104be0 = 0x18000;
    break;
  }
}
