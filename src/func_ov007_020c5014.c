extern int _ZN4cstd3divEii(int, int);

#define AMAT(p) (*(int*)((long long)(int)(p) & 0xffffffffffffffffLL))

#pragma opt_common_subs off
int func_ov007_020c5014(char* c, int x) {
  char* obj;
  int i;
  int j;
  int* arr;
  int d, den;
  *(unsigned short*)(c + 0xb4) = 0x1000;
  i = *(int*)(c + 0xac);
  if (i >= *(int*)(c + 8)) goto ret2;
  obj = (*(char***)(c + 0x38))[i];
  j = *(int*)(c + 0xb0);
  if (x >= (*(int**)(obj + 0x20))[j]) {
    for (;;) {
      AMAT(c + 0xb0) += 1;
      j = *(int*)(c + 0xb0);
      if (j >= *(unsigned short*)(obj + 8)) {
        AMAT(c + 0xac) += 1;
        {
          int cnt = *(int*)(c + 8);
          if (*(int*)(c + 0xac) >= cnt) {
            *(int*)(c + 0xac) = cnt;
            *(int*)(c + 0xb0) = *(unsigned short*)(obj + 8);
            return 2;
          }
        }
        *(int*)(c + 0xb0) = 0;
        return 1;
      }
      if (x < (*(int**)(obj + 0x20))[j]) break;
    }
  }
  if (j <= 0) goto ret0;
  arr = *(int**)(obj + 0x20);
  d = arr[j] - x;
  den = arr[j] - arr[j - 1];
  if (den == 0) den = 1;
  *(unsigned short*)(c + 0xb4) = 0x1000 - _ZN4cstd3divEii(d << 12, den);
ret0:
  return 0;
ret2:
  return 2;
}
