#include "common.h"
extern void func_02012790(int a);
extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int i);
extern int func_ov003_020adec0(char *c, unsigned int r6);
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern int data_0208ee44;
extern u8 data_0209caa0[];
extern u8 data_02092128;
extern u8 data_02092114;
extern u16 data_020a0e58[];
extern u16 data_020a0e5a[];
#pragma opt_common_subs off
void func_ov003_020ae358(char *c)
{
  int nxt;
  u8 idx = data_020a0e40;
  int unlocked = 0;
  int i;
  u16 keys;
  u16 timer;
  int cur;
  if (data_020a0de8[idx * 4] != 0) {
    unlocked = data_020a0de9[idx * 4] != 0;
  }
  if (unlocked) {
    if ((u8)(data_020a0dea[idx * 4] - 0x58) < 0x50 &&
        (u8)(data_020a0deb[idx * 4] - *(u8 *)(c + 0x12b) + 0x28) < 0x50) {
      *(u8 *)(c + 0x133) = 2;
      *(u8 *)(c + 0x132) = 3;
      *(u8 *)(c + 0x118) = (u8)(data_0208ee44 * 6);
      *(u8 *)(c + 0x139) = 1;
      *(u8 *)(c + 0x119) = 0x10;
      func_02012790(data_0209caa0[0x41] + 0x3c);
      return;
    }
    if (*(u8 *)(c + 0x130) <= 1) return;
    if (data_0209caa0[0x41] != 3) return;
    for (i = 0; i < 3; i++) {
      if (_ZN8SaveData19IsCharacterUnlockedEj((unsigned int)i) != 0) {
        volatile u8 *ep = &data_020a0e40;
        u8 t = *ep;
        u8 *p = (u8 *)(c + i);
        u8 *row = &data_020a0de8[t * 4];
        u16 dx = (u16)(row[2] - p[0x124] + 0x18);
        if (dx < 0x30) {
          u16 dy = (u16)(row[3] - p[0x128] + 0x18);
          if (dy < 0x2b) {
            *(u8 *)(c + 0x133) = 1;
            *(u8 *)(c + 0x134) = (u8)func_ov003_020adec0(c, (unsigned int)i);
            data_02092128 = (u8)i;
            data_02092114 = (u8)i;
            *(u8 *)(c + 0x132) = (u8)i;
            *(u8 *)(c + 0x118) = (u8)(data_0208ee44 * 3);
            *(u8 *)(c + 0x139) = 2;
            *(u8 *)(c + 0x119) = 0x10;
            func_02012790(data_0209caa0[0x41] + 0x3c);
            return;
          }
        }
      }
    }
    return;
  }
  if (data_0209caa0[0x42] != 0) goto zero_timer;
  keys = data_020a0e58[0];
  if ((keys & 0x30) == 0) goto zero_timer;
  timer = *(u16 *)(c + 0x106);
  if (timer != 0) {
    *(u16 *)(((long long)(int)(c + 0x106)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    return;
  }
  if (*(u8 *)(c + 0x135) == 0) return;
  if (*(u8 *)(c + 0x133) != 1) return;
  if (*(u8 *)(c + 0x130) < 3) return;
  cur = *(u8 *)(c + 0x134);
  nxt = cur;
  if (keys & 0x20) {
    int pressed = data_020a0e58[1] & 0x20;
    if (pressed == 0) {
      if (timer != 0) goto cmp_cur;
    }
    cur = idx << 2;
    cur = (*(u16 *)((char *)data_020a0e5a + cur) & 0x20) ? 0x10 : 8;
    *(u16 *)(c + 0x106) = cur;
    cur = *(u8 *)(c + 0x134);
    if (cur != 0) nxt = (u8)(nxt - 1);
  } else if (keys & 0x10) {
    int pressed = data_020a0e58[1] & 0x10;
    if (pressed == 0) {
      if (timer != 0) goto cmp_cur;
    }
    cur = idx << 2;
    cur = (*(u16 *)((char *)data_020a0e5a + cur) & 0x10) ? 0x10 : 8;
    *(u16 *)(c + 0x106) = cur;
    {
      unsigned a = *(u8 *)(c + 0x130);
      cur = *(u8 *)(c + 0x134);
      if (cur != a - 2) nxt = (u8)(nxt + 1);
    }
  }
cmp_cur:
  if (nxt == cur) return;
  *(u8 *)(c + 0x134) = nxt;
  func_02012790(0x12e);
  return;
zero_timer:
  *(u16 *)(c + 0x106) = 0;
}
