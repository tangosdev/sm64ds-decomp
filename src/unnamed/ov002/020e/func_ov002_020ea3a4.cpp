//cpp
extern "C" signed char data_0209f310[];
extern "C" unsigned char data_0209f2d8;
extern "C" int func_ov002_020ea3a4(void* a) {
  int b = (*(unsigned short*)((char*)a+0xc) == 0xb3);
  if (b != 0) {
    unsigned char idx = *(unsigned char*)((char*)*(void**)((char*)a+0x438)+0x6d8);
    *(int*)((char*)a+0x48c) = data_0209f310[idx] + 0x19;
  } else {
    int b2 = (data_0209f2d8 == 1);
    if (b2 != 0) *(int*)((char*)a+0x48c) = 0x4f;
    else *(int*)((char*)a+0x48c) = 0x22;
  }
  return *(int*)((char*)a+0x48c);
}
