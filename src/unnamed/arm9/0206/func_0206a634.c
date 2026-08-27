extern int (*data_020a9dec)(int);
extern void func_0206a600(void);
extern void Crash(void);

void func_0206a634(int unused, int arg)
{
    if ((arg & 0x3f) == 0x11) {
        int r = 0;
        if (data_020a9dec) r = data_020a9dec(0);
        if (r == 0) return;
        func_0206a600();
    } else {
        Crash();
    }
}
