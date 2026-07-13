typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned char u8;
typedef signed char s8;
void *_ZN5Actor10FindWithIDEj(u32 id);
void func_ov064_02118ee4(char *c)
{
  int p_addr;
  if (*(u8 *)(c + 0x338)) {
    if (*(u32 *)(c + 0x320)) {
      char *a = (char *)_ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x320));
      if (a)
        *(u8 *)(a + 0xd4) = 1;
    }
  }
  if (*(u8 *)(c + 0x339))
    return;
  p_addr = (u32)(c + 0x328);
  {
    u8 idx = *(u8 *)(c + 0x328);
    s8 *tab = *(s8 **)(c + 0x324);
    u8 *p = (u8 *)((u64)p_addr & 0xFFFFFFFFFFFFFFFFULL);
    int m1 = ~0;
    *(s8 *)(c + 0x336) = tab[idx];
    *p = *p + 1;
    if ((*(s8 **)(c + 0x324))[*(u8 *)(c + 0x328)] == (s8)m1) {
      *(u8 *)(c + 0x328) = 0;
      if (*(u32 *)(c + 0x320)) {
        char *a = (char *)_ZN5Actor10FindWithIDEj(*(u32 *)(c + 0x320));
        if (a) {
          u8 *f = (u8 *)((u64)(u32)(a + 0xd4) & 0xFFFFFFFFFFFFFFFFULL);
          *f |= 2;
        }
      }
    }
  }
  *(u8 *)(c + 0x339) = 1;
  {
    char *b = (char *)((u64)(u32)(c + 0x300) & 0xFFFFFFFFFFFFFFFFULL);
    *(unsigned short *)(b + 0x34) = 0;
  }
}
