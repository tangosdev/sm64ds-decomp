/* storage 0x023970C8 runtime 0x027C9040 size 0x244 main. */

extern void fn_023A4340(unsigned int a, unsigned int b, unsigned int *out);
extern void fn_02395B9C(unsigned int arg);
extern void fn_023956D0(unsigned int arg);
extern void fn_02395BE0(unsigned int a, unsigned int b);

int fn_023970C8(unsigned int arg1, unsigned int arg2) {
    char *base;
    char *hi;
    unsigned int tmp;
    unsigned int tmp2;
    unsigned int idx;
    unsigned int scaled;
    unsigned int flags;
    unsigned int n;
    unsigned int i;

    base = *(char **)0x0380FFF4;
    if (((1u << arg1) & *(unsigned short *)(base + 0x200 + 0xF0)) == 0)
        return 5;
    *(unsigned short *)(base + 0x300 + 0x76) = (unsigned short)arg1;
    hi = *(char **)0x0380FFF4;
    hi = hi + 0x500;
    switch (*(unsigned short *)(hi + 0xB0)) {
    case 0:
        break;
    case 1:
        break;
    case 2:
    case 5:
        tmp = 0;
        idx = arg1 - 1;
        scaled = idx * 6;
        fn_023A4340(scaled + 0xF2, 3, &tmp);
        fn_02395B9C(tmp);
        fn_023A4340(scaled + 0xF5, 3, &tmp);
        fn_02395B9C(tmp);
        if (arg2 != 0)
            fn_023956D0(0x5DC);
        tmp = 0;
        flags = *(unsigned int *)(*(char **)0x0380FFF4 + 0x5BC);
        if ((flags & 0x10000) != 0) {
            if ((flags & 0x8000) == 0) {
                fn_023A4340(idx + 0x154, 1, &tmp);
                tmp = *(unsigned int *)(*(char **)0x0380FFF4 + 0x5BC) | ((tmp & 0x1F) << 10);
                fn_02395B9C(tmp);
            }
        } else {
            fn_023A4340(idx + 0x146, 1, &tmp);
            fn_02395BE0(0x1E, tmp);
        }
        break;
    case 3:
        n = *(unsigned short *)(hi + 0xB4) + 0xCF;
        i = 0;
        while (i < *(unsigned short *)(*(char **)0x0380FFF4 + 0x500 + 0xB8)) {
            tmp = 0;
            tmp2 = 0;
            fn_023A4340(n, 1, &tmp2);
            fn_023A4340(n + arg1, 1, &tmp);
            fn_02395BE0(tmp2, tmp);
            n = n + 0xF;
            i = i + 1;
        }
        i = 0;
        while (i < *(unsigned short *)(*(char **)0x0380FFF4 + 0x500 + 0xB6)) {
            tmp = 0;
            fn_023A4340(n, 1, &tmp);
            tmp <<= 8;
            fn_023A4340(n + arg1, 1, &tmp);
            tmp |= 0x50000;
            fn_02395B9C(tmp);
            n = n + 0xF;
            i = i + 1;
        }
        if (arg2 != 0)
            fn_023956D0(0x5DC);
        break;
    case 4:
        break;
    default:
        break;
    }
    return 0;
}
