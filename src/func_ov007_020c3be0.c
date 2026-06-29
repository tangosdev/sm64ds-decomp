#define BLEND(out, t, a, b, mask, shift) \
  do { \
    if ((t) >= 0x1000) (out) = ((b) & (mask)) >> (shift); \
    else if ((t) <= 0) (out) = ((a) & (mask)) >> (shift); \
    else { \
      int cb = ((b) & (mask)) >> (shift); \
      int ca = ((a) & (mask)) >> (shift); \
      (out) = (int)(((long long)(0x1000 - (t)) * (ca << 0xc) + (long long)(t) * (cb << 0xc)) >> 0xc) >> 0xc; \
    } \
  } while (0)

unsigned short func_ov007_020c3be0(int t, int a, int b){
  int rr, gg, bb;
  BLEND(rr, t, a, b, 0x1f, 0);
  BLEND(gg, t, a, b, 0x3e0, 5);
  BLEND(bb, t, a, b, 0x7c00, 0xa);
  return (unsigned short)(rr | (gg << 5) | (bb << 0xa));
}
