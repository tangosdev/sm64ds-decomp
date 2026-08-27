typedef short s16;

extern void func_02052208(void *obj);
extern void func_0204ffcc(char *self);
extern void func_02052438(char *self);
extern int func_02052458(int *p);
extern void func_0204fe5c(char *obj, int arg1);
extern int func_02052420(int *o);

extern char data_020a5bd4[];
extern s16 data_02086384[];

void func_020522c4(void)
{
    int i;
    int t1;
    char *r = data_020a5bd4;
    for (i = 0; i < 4; i++) {
        int flags = *(int *)(r + 0xf0);
        if ((flags << 31) >> 31) {
            if (*(int *)(r + 0xf4) != 0) {
                func_02052208(r);
            } else {
                if ((flags << 29) >> 31) {
                    if (*(int *)(r + 0xf8) != 0) {
                        func_0204ffcc(r);
                        {
                            int *flagsp = (int *)(r + 0xf0);
                            *flagsp |= 2;
                            *flagsp &= ~4;
                        }
                    }
                }
                if ((*(int *)(r + 0xf0) << 30) >> 31) {
                    unsigned long t2;
                    func_02052438(r + 0xc8);
                    t1 = data_02086384[*(int *)(r + 0x11c)];
                    t2 = data_02086384[func_02052458((int *)(r + 0xc8)) >> 8];
                    t1 = t2 + t1;
                    if (t1 != *(int *)(r + 0x120)) {
                        func_0204fe5c(r, t1);
                        *(int *)(r + 0x120) = t1;
                    }
                    if ((*(int *)(r + 0xf0) << 28) >> 31) {
                        if (func_02052420((int *)(r + 0xc8))) {
                            func_02052208(r);
                        }
                    }
                }
            }
        }
        r += 0x12c;
    }
}
