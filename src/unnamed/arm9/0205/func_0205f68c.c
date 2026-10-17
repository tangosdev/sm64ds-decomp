extern int func_0205fa78(int a);
extern int func_0205fab4(int x, int y, int z);
extern int data_020a8110;

int func_0205f68c(int mode, int arg1, int arg2, int arg3) {
    switch (mode) {
    case 1:
        if (arg2 == 0) {
            if ((unsigned int)(*(int *)0x27ffc3c - data_020a8110) <= 7)
                return 0;
        }
        if (arg1 != 0) {
            if (arg3 != 0)
                func_0205fa78(arg1);
            else
                func_0205fab4(arg1, 0, 0);
        }
        *(volatile unsigned short *)0x4000304 |= 1;
        break;
    case 0:
        *(volatile unsigned short *)0x4000304 &= ~1;
        data_020a8110 = *(int *)0x27ffc3c;
        if (arg1 != 0) {
            if (arg3 != 0)
                func_0205fa78(arg1);
            else
                func_0205fab4(arg1, 0, 0);
        }
        break;
    }
    return 1;
}
