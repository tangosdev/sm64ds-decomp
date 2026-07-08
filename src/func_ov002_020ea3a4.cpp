//cpp
extern "C" signed char NUM_VS_STARS_OBTAINED_PLAYER[];
extern "C" unsigned char CURRENT_GAMEMODE;
extern "C" int func_ov002_020ea3a4(void* a) {
  int b = (*(unsigned short*)((char*)a+0xc) == 0xb3);
  if (b != 0) {
    unsigned char idx = *(unsigned char*)((char*)*(void**)((char*)a+0x438)+0x6d8);
    *(int*)((char*)a+0x48c) = NUM_VS_STARS_OBTAINED_PLAYER[idx] + 0x19;
  } else {
    int b2 = (CURRENT_GAMEMODE == 1);
    if (b2 != 0) *(int*)((char*)a+0x48c) = 0x4f;
    else *(int*)((char*)a+0x48c) = 0x22;
  }
  return *(int*)((char*)a+0x48c);
}
