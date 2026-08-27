/* func_ov007_020b6208 at 0x020b6208
 * Maps an input code to a result code via a jump-table switch, then tail-calls
 * func_ov007_020bdeb0 with it.
 */

extern char *data_ov007_0210342c;
extern int func_ov007_020bdeb0(int a);

int func_ov007_020b6208(int code)
{
    int r1 = 0x14;

    switch (code) {
    case 0:
        r1 = 0;
        break;
    case 1:
        r1 = 1;
        break;
    case 2:
        r1 = 2;
        break;
    case 14:
    case 15:
    case 16: {
        char *p0 = data_ov007_0210342c;
        int r3 = code - 0xe;
        switch (*(short *)(*(char **)(p0 + 8))) {
        case 0:
            r1 = 6;
            break;
        case 2:
        case 3:
            r1 = 7;
            break;
        case 7: {
            int f58 = *(int *)(p0 + 0x58);
            if (f58 == -1)
                r1 = 8;
            else
                r1 = (r3 == f58) ? 0xc : 9;
            break;
        }
        case 9:
            if (*(int *)(p0 + 0x58) == -1)
                r1 = 0xd;
            break;
        case 1:
            r1 = 0x12;
            break;
        }
        break;
    }
    case 21:
        r1 = 0x13;
        break;
    case 20:
        r1 = 0x15;
        break;
    case 9:
    case 10:
    case 11:
        r1 = 0x16;
        break;
    case 12: {
        char *p0 = data_ov007_0210342c;
        if (*(unsigned char *)(*(char **)(*(char **)(p0 + 0x28)) + 9) != 0)
            r1 = 0x19;
        else
            r1 = 0x18;
        break;
    }
    case 13: {
        char *p0 = data_ov007_0210342c;
        if (*(unsigned char *)(*(char **)(*(char **)(p0 + 0x28)) + 9) == 0)
            r1 = 0x19;
        else
            r1 = 0x18;
        break;
    }
    case 8:
        r1 = 0x17;
        break;
    case 22:
    case 23: {
        char *p0 = data_ov007_0210342c;
        switch (*(short *)(*(char **)(p0 + 8))) {
        case 7:
            r1 = (code == 22) ? 0xa : 0xb;
            break;
        case 9:
            r1 = (code == 22) ? 0xe : 0xf;
            break;
        }
        break;
    }
    }

    return func_ov007_020bdeb0(r1);
}
