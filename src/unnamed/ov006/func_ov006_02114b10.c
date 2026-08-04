extern void func_ov004_020afdd0(int a0, int a1, int a2, int a3, int a4);
extern int data_ov006_021387a4[];
extern int data_ov006_02138364[];

void func_ov006_02114b10(char* self)
{
  int i; char* p = self;
  for (i = 0; i < 0x40; i++) {
    if (*(unsigned char*)(p + 0x5000 + 0x9bc) != 0) {
      if ((*(int*)(self + 8) & 0xff) == 0) {
        int idx = *(int*)(p + 0x5000 + 0x9b4) / 4 + 8;
        int b = *(unsigned char*)(p + 0x5000 + 0x9bd);
        if (b != 0) idx += 4;
        func_ov004_020afdd0(data_ov006_02138364[idx],
                            *(int*)(p + 0x5000 + 0x9a4) >> 12,
                            *(int*)(p + 0x5000 + 0x9a8) >> 12, -1, 0);
      } else {
        int idx = *(int*)(p + 0x5000 + 0x9b4) / 4 + 4;
        int b = *(unsigned char*)(p + 0x5000 + 0x9bd);
        if (b != 0) idx += 4;
        func_ov004_020afdd0(data_ov006_021387a4[idx],
                            *(int*)(p + 0x5000 + 0x9a4) >> 12,
                            *(int*)(p + 0x5000 + 0x9a8) >> 12, -1, 0);
      }
    }
    p += 0x24;
  }
}
