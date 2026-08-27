extern int func_ov006_021146ac(void* p);
extern void func_ov006_0211470c(int* a, void* b);
extern long long func_0203d5bc(int* p);

struct A { int f0; int f4; };

#pragma opt_strength_reduction off
int func_ov006_02112190(char* sl, struct A* sb)
{
  int i;
  void* base;
  int n;
  int r7;
  int delta[2];
  int out[2];
  if (sb->f4 >= 0x90000) return 0;
  base = *(void**)(sl + 4);
  n = *(int*)((char*)base + 0x4670);
  i = 0;
  if (n > 0) {
    do {
      if (*(unsigned char*)(sl + 0xce + i)) {
        r7 = func_ov006_021146ac(i >= n
            ? 0 : *(void**)((char*)base + i * 4 + 0x46bc));
        if (r7 > 0) {
          delta[0] = sb->f0;
          delta[1] = sb->f4;
          func_ov006_0211470c(out, i >= *(int*)((char*)*(void**)(sl + 4) + 0x4670)
              ? 0 : *(void**)((char*)*(void**)(sl + 4) + i * 4 + 0x46bc));
          delta[0] = delta[0] - out[0];
          delta[1] = delta[1] - out[1];
          if (func_0203d5bc(delta) < (long long)r7) {
            void* e = i >= *(int*)((char*)*(void**)(sl + 4) + 0x4670)
                ? 0 : *(void**)((char*)*(void**)(sl + 4) + i * 4 + 0x46bc);
            *(unsigned char*)((char*)e + 0x38) = 1;
            return 1;
          }
        }
      }
      i++;
      base = *(void**)(sl + 4);
      n = *(int*)((char*)base + 0x4670);
    } while (i < n);
  }
  return 0;
}
