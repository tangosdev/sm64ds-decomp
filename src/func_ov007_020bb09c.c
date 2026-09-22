extern int func_ov007_020bdeb0(int a);
extern int func_ov007_020c1da0(int i);

extern char* data_ov007_0210342c;
extern char* data_ov007_02104ba0;
extern char* data_ov007_02104b9c;

#define G data_ov007_0210342c
#define BA0 data_ov007_02104ba0
#define B9C data_ov007_02104b9c

int func_ov007_020bb09c(void)
{
    char* r5 = *(char**)((char*)BA0);
    char* r4 = *(char**)((char*)G + 0x54);
    unsigned short r2 = *(unsigned short*)r4;
    int r3 = 0;

    if ((r2 & 0x400) && !(*(unsigned short*)(r4 + 2) & 0x400)) {
        r3 = 1;
    }
    if (r3 != 0 || ((r2 & 0x40) && !(*(unsigned short*)(r4 + 2) & 0x40))) {
        int r1;
        int ip;
        char* r3p = *(char**)((char*)BA0 + 0x44);
        r1 = *(int*)(r3p + 0x18);
        switch (r1) {
        case 0:      ip = 0x800; break;
        case 0x800:  ip = 0x1000; break;
        case 0x1000: ip = -1; break;
        default:     ip = 0; break;
        }
        if (ip != -1) {
            int t = *(int*)((char*)BA0 + 0x3c);
            *(int*)(r3p) = 1;
            *(int*)(r3p + 4) = 1;
            *(short*)(r3p + 0xc) = 0;
            *(int*)(r3p + 8) = 0x100;
            *(int*)(r3p + 0x14) = t;
            *(int*)(*(int*)(r3p + 0x10)) = *(int*)(r3p + 0x14);
            *(int*)(r3p + 0x18) = ip;
            func_ov007_020bdeb0(0x23);
        }
    } else if (((r2 & 2) && !(*(unsigned short*)(r4 + 2) & 2)) ||
               ((r2 & 0x80) && !(*(unsigned short*)(r4 + 2) & 0x80))) {
        int r1;
        int ip;
        char* r3p = *(char**)((char*)BA0 + 0x44);
        r1 = *(int*)(r3p + 0x18);
        switch (r1) {
        case 0:      ip = -1; break;
        case 0x800:  ip = 0; break;
        case 0x1000: ip = 0x800; break;
        default:     ip = 0; break;
        }
        if (ip != -1) {
            int t = *(int*)((char*)BA0 + 0x3c);
            *(int*)(r3p) = 1;
            *(int*)(r3p + 4) = 1;
            *(short*)(r3p + 0xc) = 0;
            *(int*)(r3p + 8) = 0x100;
            *(int*)(r3p + 0x14) = t;
            *(int*)(*(int*)(r3p + 0x10)) = *(int*)(r3p + 0x14);
            *(int*)(r3p + 0x18) = ip;
            func_ov007_020bdeb0(0x22);
        }
    }

    {
        int r3b = 0;
        char* r2p = *(char**)((char*)G + 0x54);
        unsigned short r1 = *(unsigned short*)r2p;
        if ((r1 & 0x100) && !(*(unsigned short*)(r2p + 2) & 0x100)) {
            r3b = 1;
        }
        if (r3b != 0 || ((r1 & 0x200) && !(*(unsigned short*)(r2p + 2) & 0x200))) {
            if (*(int*)(*(char**)(((char*)B9C) + 8) + 0x50) == 0x1f000) {
                *(short*)(*(char**)((char*)BA0 + 8) + 2) = 3;
                func_ov007_020bdeb0(0x29);
            } else {
                *(short*)(*(char**)((char*)BA0 + 8) + 2) = 4;
                func_ov007_020bdeb0(0x2a);
            }
        }
    }

    {
        char* r1 = *(char**)((char*)G + 0x50);
        if (*(unsigned short*)(r1 + 0xc) != 0) {
            return *(unsigned short*)(r1 + 0xc);
        }
        if ((unsigned int)*(int*)(r1 + 0x24) < 1U) {
            return *(int*)(r1 + 0x24);
        }
    }

    {
        int rv = func_ov007_020c1da0(0);
        if (rv != 0) {
            return rv;
        }
    }

    {
        char* b = *(char* volatile*)&data_ov007_02104ba0;
        short st = *(short*)(*(char**)(b + 8));
        if (st == 6) return st;
        if (st == 7) return st;
        if (*(int*)(b + 0x30) != 0) {
            return *(int*)(b + 0x30);
        }
    }

    {
        unsigned short r1 = *(unsigned short*)r4;
        int r2b = 0;
        if ((r1 & 0x800) && !(*(unsigned short*)(r4 + 2) & 0x800)) {
            r2b = 1;
        }
        if (r2b != 0 || ((r1 & 0x20) && !(*(unsigned short*)(r4 + 2) & 0x20))) {
            int n = *(int*)(r5 + 8);
            if (n <= 0) return n;
            {
                char* e = *(char**)(*(char**)(r5 + 0x38) + (n - 1) * 4);
                int* p14 = (int*)(r5 + 0x14);
                int* p8 = (int*)(r5 + 8);
                *p14 = *p14 - *(unsigned short*)(e + 0xa);
                *p8 = *p8 - 1;
                *(short*)(*(char**)((char*)BA0 + 8) + 2) = 6;
                return func_ov007_020bdeb0(0x20);
            }
        }
        if ((r1 & 1) && !(*(unsigned short*)(r4 + 2) & 1)) {
            /* fallthrough to block_66 */
        } else {
            if (!(r1 & 0x10)) return r1 & 0x10;
            if (*(unsigned short*)(r4 + 2) & 0x10) return *(unsigned short*)(r4 + 2) & 0x10;
        }
    }

    {
        char* b = *(char* volatile*)&data_ov007_02104ba0;
        int n = *(int*)(r5 + 8);
        int lim = *(int*)(b + 0x2c);
        if (n >= lim) return lim;
        {
            char* e = *(char**)(*(char**)(r5 + 0x38) + n * 4);
            int* p14 = (int*)(r5 + 0x14);
            int* p8 = (int*)(r5 + 8);
            *p14 = *p14 + *(unsigned short*)(e + 0xa);
            *p8 = *p8 + 1;
            *(short*)(*(char**)(*(char* volatile*)&data_ov007_02104ba0 + 8) + 2) = 7;
            return func_ov007_020bdeb0(0x21);
        }
    }
}
