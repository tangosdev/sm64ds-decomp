// NONMATCHING: predicated-select register/direction: `if(r<c) r=c` emits `movge r2,r1;mov r0,r2` but ROM has `movlt r1,r2;mov r0,r1`. The fixed-point result stays in r1 in the ROM; every clamp phrasing (ternary, override, early-return) either collapses the mov (in-place r0) or picks r2. (div=2)
/* func_ov002_020bf224 at 0x020bf224 (ov002), size 0x58
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc */
extern unsigned char data_020a0e40[];
extern short data_0209f4a0[];
int func_ov002_020bf224(int a, int b, int c) {
  long long p = (long long)b * *(short*)((char*)data_0209f4a0 + data_020a0e40[0]*0x18);
  int r = (int)((p + 0x800) >> 12);
  return r < c ? c : r;
}
