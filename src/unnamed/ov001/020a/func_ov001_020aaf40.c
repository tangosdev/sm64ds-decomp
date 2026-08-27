extern void func_ov001_020aa6b0(void *node, int v);
extern void func_ov001_020aa420(void);
extern int _ZN6Player8HasNoCapEv(void *p);
extern int _ZN6Player15IsCollectingCapEv(void *p);
extern void func_ov001_020aadac(void);
extern void func_ov001_020aaa54(void);

extern unsigned char data_ov001_020ad620;
extern char *data_ov001_020ad634[3];
extern unsigned char data_0209f284;
extern unsigned char data_0209f2d8;
extern char *data_0209f394;
extern unsigned char data_ov001_020ad624;
extern unsigned char data_ov001_020ad62c[3];

enum { false, true };

void func_ov001_020aaf40(void)
{
    int j;
    int i;

    if (data_ov001_020ad620 != 0) {
        data_ov001_020ad620 -= 1;
        for (i = 0; i < 3; i++) {
            char *node = data_ov001_020ad634[i];
            while (node) {
                if (*(int*)(node + 0x14) != -1 && *(unsigned char*)(node + 0x1a) != 0) {
                    int v;
                    *(unsigned char *)(int)(node + 0x1a) -= 1;
                    v = *(unsigned char*)(node + 0x1a);
                    if (v == 0) {
                        func_ov001_020aa6b0(node, 1);
                    } else if (v % 10 == 0) {
                        func_ov001_020aa6b0(node, ((v / 10) & 1) == 0 ? 1 : 0);
                    }
                }
                node = *(char**)(node + 0x10);
            }
        }
        if (data_ov001_020ad620 == 0)
            data_0209f284 = 0;
    }

    {
        int t;
        t = data_0209f2d8;
        t = t == 1;
        if (t != false) {
            func_ov001_020aa420();
            return;
        }
    }

    {
        char *pl = data_0209f394;
        if (_ZN6Player8HasNoCapEv(pl) && !_ZN6Player15IsCollectingCapEv(pl)) {
            func_ov001_020aadac();
            data_ov001_020ad624 = 1;
            return;
        }
        for (j = 0; j < 3; j++) {
            if (data_ov001_020ad62c[j] & 2)
                data_ov001_020ad62c[j] &= ~3;
        }
        func_ov001_020aaa54();
        data_ov001_020ad624 = 0;
    }
}
