typedef short s16;
extern void func_ov092_021313b0(char* c);

void func_ov092_021314d0(char* c) {
  if (*(unsigned char*)(c+0x574) == 3) {
    func_ov092_021313b0(c);
  } else {
    *(int*)(((int)c + 0x570)) = *(int*)(((int)c + 0x570)) + 1;
    int* arr = *(int**)(c+0x56c);
    int v = arr[*(int*)(c+0x570)];
    if (v != -1) {
      *(int*)(c+0x568) = v;
    } else {
      *(int*)(c+0x568) = arr[0];
      *(int*)(c+0x570) = 0;
    }
  }
  *(unsigned char*)(c+0x575) = (*(s16*)(c+0x92) >> 0xe) & 3;
  {
    unsigned char* f = (unsigned char*)(((int)c + 0x575));
    *f = *f | ((*(s16*)(c+0x94) >> 0xc) & 0xc);
    *f = *f | ((*(s16*)(c+0x96) >> 0xa) & 0x30);
  }
}
