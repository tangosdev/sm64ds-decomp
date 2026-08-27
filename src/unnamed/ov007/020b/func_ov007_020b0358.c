extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020bca40(int i);
extern void func_ov007_020bcba8(int i);
extern void func_ov007_020b283c(void);
extern void func_ov007_020b76a0(void);
extern void func_ov007_020bd1c0(int i);
extern void *func_ov007_020b8f78(void);
extern char *data_ov007_0210342c;

void func_ov007_020b0358(void)
{
    char *b = data_ov007_0210342c;
    char *s2 = *(char**)(b + 8);
    int state = *(short*)s2;
    int r5 = *(int*)(s2 + 0xc);

    switch (state) {
    case 0:
        if (r5 == 0) {
            *(int*)(b + 0x18) = 1;
            func_ov007_020c1d8c(0);
            return;
        }
        if (r5 != 1)
            return;
        *(int*)(b + 0x1c) = 3;
        func_ov007_020bca40(1);
        *(short*)(*(char**)(data_ov007_0210342c + 8) + 2) = 1;
        return;
    case 1:
        if (r5 == 0) {
            func_ov007_020bcba8(2);
            return;
        }
        if (r5 != 1)
            return;
        *(int*)(b + 0x1c) = 4;
        func_ov007_020bca40(2);
        *(short*)(*(char**)(data_ov007_0210342c + 8) + 2) = 2;
        return;
    case 2: {
        int hit = 0;
        int st2;
        if (r5 == 0) {
            func_ov007_020b283c();
            func_ov007_020b76a0();
            func_ov007_020bd1c0(1);
        }
        if (r5 == 0x28) {
            *(short*)(*(char**)(data_ov007_0210342c + 0xc) + 2) = 7;
        }
        st2 = *(int*)(data_ov007_0210342c + 0x20);
        switch (st2) {
        case 1:
            if (*(short*)func_ov007_020b8f78() == 8) {
                *(short*)(*(char**)(data_ov007_0210342c + 4) + 2) = 2;
                hit = 1;
            }
            break;
        case 0:
            if (*(short*)func_ov007_020b8f78() == 5) {
                *(short*)(*(char**)(data_ov007_0210342c + 4) + 2) = 3;
                hit = 1;
            }
            break;
        }
        if (hit) {
            *(int*)(data_ov007_0210342c + 0x1c) = 5;
        }
        return;
    }
    }
}
