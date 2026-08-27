int func_ov002_020c1e44(void* a, int x) {
  *(short*)((char*)a+0x94) = (short)((x << 1) - *(short*)((char*)a+0x94) + 0x8000);
  *(int*)((char*)a+0x98) = (int)(((long long)*(int*)((char*)a+0x98) * 0xe00 + 0x800) >> 12);
  if (*(int*)((char*)a+0x98) < 0x4000) *(int*)((char*)a+0x98) = 0x4000;
  return 1;
}
