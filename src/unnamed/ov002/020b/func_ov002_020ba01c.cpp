//cpp
typedef int Fix12i;
extern "C" Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);
extern "C" void func_ov002_020ba01c(char* c, int mask, Fix12i b, Fix12i base, Fix12i target){
  Fix12i d = _ZN4cstd4fdivEii((Fix12i)*(unsigned short*)(c+0x338), b);
  Fix12i diff = target - base;
  Fix12i e = (Fix12i)(((long long)diff * d + 0x800) >> 12);
  Fix12i v = base + _ZN4cstd4fdivEii(e, 0x1000);
  Fix12i h = (Fix12i)(((long long)v * 0x3c000 + 0x800) >> 12);
  *(Fix12i*)(c+0x32c) = 0x3c000 - h;
  if (mask & 1) *(Fix12i*)(c+0x320) = v;
  if (mask & 2) *(Fix12i*)(c+0x324) = v;
  if (mask & 4) *(Fix12i*)(c+0x328) = v;
}
