typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern void func_ov007_020bdeb0(int a);
extern char* data_ov007_02103360;
extern char* data_ov007_0210342c;

int func_ov007_020b3708(void)
{
    int ip;
    int r3 = 1;
    int r2;
    char *p;
    int r4;

    p = *(char**)data_ov007_02103360;
    r2 = *(int*)(data_ov007_02103360 + 0x14);
    ip = *(short*)(p + 0);
    r4 = *(short*)(p + 2);

    switch (ip) {
    case 0xff:
        r3 = 0;
        break;
    case 0:
        if (r2 == 2) r4 = 2;
        else if (r2 == 3) r4 = r3;
        else if (r2 == 0) r4 = 7;
        break;
    case 1:
        if (r2 == 2) r4 = 0;
        else if (r2 == 3) r4 = 2;
        else if (r2 == 0) r4 = 7;
        break;
    case 2:
        if (r2 == 2) r4 = r3;
        else if (r2 == 3) r4 = 0;
        else if (r2 == 0) r4 = 7;
        break;
    case 7:
        if (r2 == 1) {
            r4 = *(short*)(p + 0xa);
        }
        break;
    case 8:
        if (r2 == 0) {
            r4 = 0xfd;
        }
        break;
    case 9:
        if (r2 == 1) {
            r4 = 0xfd;
        } else if (r2 == 2 || *(int*)(data_ov007_02103360 + 0x10) != 0) {
            r3 = 0;
            *(int*)(data_ov007_02103360 + 0x10) = 0;
            r4 = 0xb;
        } else if (r2 == 3) {
            r3 = 0;
            *(int*)(data_ov007_02103360 + 0x10) = 0;
            r4 = 0xa;
        }
        break;
    case 0xa:
    case 0xb:
        if (*(int*)(p + 0xc) == 0) {
            *(int*)(data_ov007_02103360 + 0x10) = 1;
            r4 = 9;
        }
        break;
    case 0xc:
    case 0xd:
        if (r2 == 0) {
            r4 = 9;
        } else if (r2 == 1) {
            r4 = 8;
        } else if (*(int*)(p + 0xc) == 0 && (u16)(s16)(*(short*)(p + 0xa) - 0xc) <= 1) {
            *(int*)(data_ov007_02103360 + 0x10) = 1;
        } else {
        int t;
        if (r2 == 2 || ((t = *(int*)(data_ov007_02103360 + 0x10)) != 0 && ip == 0xc)) {
            *(short*)(p + 0) = 0xc;
            r3 = 0;
            *(int*)(data_ov007_02103360 + 0x10) = 0;
            r4 = 0xd;
        } else if (r2 == 3 || (t != 0 && ip == 0xd)) {
            *(short*)(p + 0) = 0xd;
            r3 = 0;
            *(int*)(data_ov007_02103360 + 0x10) = 0;
            r4 = 0xc;
        }
        }
        break;
    case 0xe:
        if (r2 == 2) r4 = 0xf;
        else if (r2 == 3) r4 = 0x10;
        else if (r2 == 1) r4 = 0xfe;
        break;
    case 0xf:
        if (r2 == 2) r4 = 0x10;
        else if (r2 == 3) r4 = 0xe;
        else if (r2 == 1) r4 = 0xfe;
        break;
    case 0x10:
        if (r2 == 2) r4 = 0xe;
        else if (r2 == 3) r4 = 0xf;
        else if (r2 == 1) r4 = 0xfe;
        break;
    case 0x11:
        if (r2 == 2) r4 = 0x12;
        else if (r2 == 3) r4 = 0x13;
        else if (r2 == 0) r4 = 0xe;
        else if (r2 == 1) r4 = 0x14;
        break;
    case 0x12:
        if (r2 == 2) r4 = 0x13;
        else if (r2 == 3) r4 = 0x11;
        else if (r2 == 0) r4 = 0xf;
        else if (r2 == 1) r4 = 0x14;
        break;
    case 0x13:
        if (r2 == 2) r4 = 0x11;
        else if (r2 == 3) r4 = 0x12;
        else if (r2 == 0) r4 = 0x10;
        else if (r2 == 1) r4 = 0x14;
        break;
    case 0x14:
        if (r2 == 3 && *(short*)(*(char**)(data_ov007_0210342c + 8)) == 0) {
            r4 = 0x15;
        } else if (r2 == 0) {
            if (*(short*)(*(char**)(data_ov007_0210342c + 8)) == 1) {
                r4 = 0x13;
            } else {
                r4 = *(short*)(p + 0xa);
                if ((u16)(s16)(r4 - 0xe) > 2) {
                    r4 = 0x10;
                }
            }
        }
        break;
    case 0x15:
        if (r2 == 0) {
            r4 = *(short*)(p + 0xa);
            if ((u16)(s16)(r4 - 0xe) > 2) {
                r4 = 0xf;
            }
        } else if (r2 == 2) {
            r4 = 0x14;
        }
        break;
    case 0x16:
        if (r2 == 2) {
            r4 = 0x17;
        }
        break;
    case 0x17:
        if (r2 == 3) {
            r4 = 0x16;
        }
        break;
    }

    if (r4 != 0xfd) {
        if (r4 == 0xfe) {
            int st = *(short*)(*(char**)(data_ov007_0210342c + 8));
            switch (st) {
            case 0:
                r4 = (ip == 0x10) ? 0x14 : 0x15;
                break;
            case 1:
                switch (ip) {
                case 0xe: r4 = 0x11; break;
                case 0xf: r4 = 0x12; break;
                case 0x10: r4 = 0x13; break;
                }
                break;
            default:
                r4 = 0x14;
                break;
            }
        }
    } else {
        u8 b = *(u8*)(*(char**)(*(char**)(data_ov007_0210342c + 0x28)) + 9);
        r4 = (b != 0) ? 0xc : 0xd;
    }

    if (*(short*)(*(char**)data_ov007_02103360 + 2) != r4) {
        if (r2 == 2) {
            *(short*)(data_ov007_02103360 + 0xc) = 1;
            *(short*)(data_ov007_02103360 + 0xe) = 0;
        } else if (r2 == 3) {
            *(short*)(data_ov007_02103360 + 0xc) = -1;
            *(short*)(data_ov007_02103360 + 0xe) = 0;
        } else if (r2 == 0) {
            *(short*)(data_ov007_02103360 + 0xc) = 0;
            *(short*)(data_ov007_02103360 + 0xe) = -1;
        } else if (r2 == 1) {
            *(short*)(data_ov007_02103360 + 0xc) = 0;
            *(short*)(data_ov007_02103360 + 0xe) = 1;
        }
        if (r3 != 0) {
            func_ov007_020bdeb0((*(int*)(data_ov007_02103360 + 8) != 2) ? 4 : 5);
        }
    }
    return r4;
}
