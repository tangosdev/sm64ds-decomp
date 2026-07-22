#pragma opt_loop_invariants off
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern int func_02060f60(void *thiz, int a, int b);
extern void func_0205a61c(void *a, int b, unsigned int c);
extern void func_0206081c(void *thiz);

void func_0206071c(char *thiz)
{
  unsigned int n;
  char *e0;

  e0 = thiz + 0xe0;

  do
  {
    n = *((unsigned int *)(thiz + 0x20));
    if (n > 0x100) n = 0x100;
    _ZN4CP1519InvalidateDataCacheEjj((unsigned int)(thiz + 0xe0), 0x100);
    *((int *)((*((int *)thiz)) + 0xc)) = *((int *)(thiz + 0x18));
    *((char **)((*((int *)thiz)) + 0x10)) = thiz + 0xe0;
    *((unsigned int *)((*((int *)thiz)) + 0x14)) = n;
    if (func_02060f60(thiz, 6, 1) == 0) break;
    func_0205a61c(e0, *((int *)(thiz + 0x1c)), n);
    *((int *)(((int)thiz + 0x18) & 0xFFFFFFFFFFFFFFFFull)) += n;
    *((int *)(((int)thiz + 0x1c) & 0xFFFFFFFFFFFFFFFFull)) += n;
    *((int *)(((int)thiz + 0x20) & 0xFFFFFFFFFFFFFFFFull)) -= n;
  }
  while (*((int *)(thiz + 0x20)) != 0);

  func_0206081c(thiz);
}
