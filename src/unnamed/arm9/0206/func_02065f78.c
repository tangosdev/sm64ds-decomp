/* func_02065f78 at 0x02065f78
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern int *data_020a9db8;

extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern int func_02066470(unsigned char x, unsigned arg1);
extern int func_020662c0(unsigned arg);
extern int func_0206610c(void);

void func_02065f78(void)
{
    unsigned short buf[6];
    unsigned short i;
    int result;

    buf[0] = 0;
    MultiStore16(buf[0], (char *)&buf[1], 10);

    for (i = 1; i <= 0xf; i++) {
        switch (*(int *)((char *)(data_020a9db8 + (i - 1)) + 0x1000 + 0x4e8)) {
        case 2:
            buf[1] |= (1 << i);
            break;
        case 5:
            buf[2] |= (1 << i);
            break;
        case 4:
            buf[3] |= (1 << i);
            break;
        case 8:
            buf[4] |= (1 << i);
            break;
        case 11:
            buf[5] |= (1 << i);
            break;
        }
    }

    if (buf[4])
        result = func_02066470(5, buf[4]);
    else if (buf[1])
        result = func_02066470(1, buf[1]);
    else if (buf[5])
        result = func_02066470(6, buf[5]);
    else if (buf[3])
        result = func_02066470(2, buf[3]);
    else if (buf[2])
        result = func_020662c0(buf[2]);
    else
        result = func_0206610c();

    if (result == 0x15)
        func_02066470(0, 0xffff);
}
