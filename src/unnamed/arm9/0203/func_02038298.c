extern int _ZNK5dBgPi7IsValidEv(void *c);
extern int _ZNK5dBgPi16GetColliderIndexEv(void *p);
extern int func_020393ac(void *p);
extern int func_020393b4(void *p);
extern int _ZNK5dBgPi7IsHitByEiP8dActor_cP4dBgW(void *p, int a, void *b, void *c);
extern void *data_020a0c80[];

int func_02038298(void *arg)
{
    int idx; void *e; int a, b;
    if (_ZNK5dBgPi7IsValidEv(arg) == 0) return 0;
    idx = _ZNK5dBgPi16GetColliderIndexEv(arg);
    if (idx < 0) goto r;
    if (idx < 0x18) goto work;
r:  return 0;
work:
    e = data_020a0c80[idx];
    if (e == 0) return 0;
    a = func_020393ac(e);
    b = func_020393b4(e);
    return _ZNK5dBgPi7IsHitByEiP8dActor_cP4dBgW(arg, a, (void *)b, e);
}
