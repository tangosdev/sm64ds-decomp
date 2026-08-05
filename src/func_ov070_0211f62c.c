extern int _ZN9Animation8FinishedEv(void *p);
extern signed char data_0209f2f8;
extern void FlyGuy_ChangeState(void *c, void *p);
extern char data_ov070_0212359c;

int func_ov070_0211f62c(char *c)
{
    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        if (data_0209f2f8 != 0x16)
            *(int *)(((long long)(int)(c + 0x3c4))) += 0x12c000;
        *(int *)(c + 0x3d8) = 0;
        *(short *)(c + 0x3cc) = 0x5a;
        FlyGuy_ChangeState(c, &data_ov070_0212359c);
    }
    return 1;
}
