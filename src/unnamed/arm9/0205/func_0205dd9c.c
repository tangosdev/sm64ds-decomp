extern int func_0205de9c(char *arg0, int arg1, int arg2);
extern void func_0205a588(int a, int b, int c);
extern void Crash(void);
extern void func_020048d8(int a);

extern char data_0209aff8[];
extern char data_0209a7ec[];

void func_0205dd9c(char *self) {
    unsigned char flags = *(unsigned char*)(self + 0x1f);
    unsigned int r4;
    if (flags & 1) {
        r4 = *(unsigned int*)(self + 0x1c) << 8;
        r4 = r4 >> 8;
    } else {
        r4 = *(unsigned int*)(self + 8);
    }
    if (*(unsigned short*)0x27ffc40 == 2) {
        int r0 = 0;
        if (flags & 2) {
            int lim = (int)(data_0209aff8 - data_0209a7ec) / 20;
            if (*(unsigned int*)self < (unsigned int)lim) {
                char *p = data_0209a7ec + *(unsigned int*)self * 0x14;
                r0 = func_0205de9c(p, *(int*)(self + 4), r4);
            }
        }
        if (r0 == 0) {
            func_0205a588(*(int*)(self + 4), 0, r4);
            Crash();
            return;
        }
    }
    if (*(unsigned char*)(self + 0x1f) & 1) {
        func_020048d8(*(int*)(self + 4) + r4);
    }
    {
        char **p = *(char***)(self + 0x10);
        char **end = *(char***)(self + 0x14);
        while (p < end) {
            void (*fn)(void) = *(void(**)(void))p;
            if (fn != 0) fn();
            p++;
        }
    }
}
