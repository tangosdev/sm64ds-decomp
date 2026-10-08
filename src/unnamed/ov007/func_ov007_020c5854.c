
extern int func_ov007_020c704c(int elem, void *sb, void *fp, void *r7, void *r8);
typedef struct Node { int *f0; int f4; } Node;
inline long long mul_add(long long a, int b, long long c) {
  return a * b + c;
}
int func_ov007_020c5854(char *sl, int *sb, void *fp, void *r8, void *r7, int scale)
{
  long long v; int i, j, cnt = 0;
  v = mul_add(scale, sb[0], ((long long)(0x1000 - scale)) * (*((int *)(sl + 0x70))));
  *((int *)(sl + 0x70)) = (int)(v >> 12);
  v = mul_add(scale, sb[1], ((long long)(0x1000 - scale)) * (*((int *)(sl + 0x74))));
  *((int *)(sl + 0x74)) = (int)(v >> 12);

  *((int *) (sl + 0x78)) = sb[2];
  for (i = 0; i < (*((int *) (sl + 8))); i++) {
    j = 0;
    if ((*((Node ***) (sl + 0x3c)))[i]->f4 > 0) {
      do {
        if (func_ov007_020c704c((*((Node ***) (sl + 0x3c)))[i]->f0[j], sb, fp, r7, r8) == 2)
          cnt++;
        j++;
      } while (j < (*((Node ***) (sl + 0x3c)))[i]->f4);
    }
  }
  return cnt;
}
