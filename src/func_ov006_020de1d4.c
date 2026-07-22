extern char* func_020beb68;
extern void func_ov006_020dc370(void*);

void func_ov006_020de1d4(char* c) {
  if (*(int*)((c + 0x5000) + 0x1cc) != 0) {
    int* p = (int*)(int)(((long long)(int)(c + 0x51cc)) & 0xFFFFFFFFFFFFFFFFLL);
    *p = *p - 1;
    if (*(int*)((c + 0x5000) + 0x1cc) < 0)
      *(int*)((c + 0x5000) + 0x1cc) = 0;
    return;
  }
  {
    char* o = func_020beb68;
    int v = o != 0 ? *(int*)(o + 0xa8) : 0;
    if (v != 0) {
      *(unsigned char*)(c + 0x51de) = 0;
      func_ov006_020dc370(c);
    }
  }
  *(int*)(c + 0x51cc) = 0xc0;
  *(int*)(c + 0x51c8) = 5;
}
