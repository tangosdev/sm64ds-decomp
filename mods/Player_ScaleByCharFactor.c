// A deliberate divergence from the ROM, built into the game instead of
// src/game/player/Player_ScaleByCharFactor.c. See notes/rom-build.md (M3).
//
// The retail function scales a value by the current character's factor in 4.12
// fixed point. Shifting by 11 instead of 12 doubles the result. The instruction
// count is unchanged, so the function still occupies its original 0x34 bytes and
// nothing downstream of it moves.
typedef int Fix12i;
extern unsigned short data_ov002_020ff170[];
Fix12i Player_ScaleByCharFactor(void*c, Fix12i a){
  int idx = *(int*)((char*)c+8) & 0xff;
  unsigned int t = data_ov002_020ff170[idx];
  return (Fix12i)(((long long)a * (int)t + 0x800) >> 11);
}
