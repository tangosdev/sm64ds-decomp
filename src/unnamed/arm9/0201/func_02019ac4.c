enum { false, true };

extern int func_0203da3c(void);
extern void func_02019e60(void);
extern int func_0203d974(void);
extern void func_02012790(int n);
extern void func_02019dfc(void);
extern void func_02011dc0(void);
extern int func_0205f66c(int n);
extern int func_0205f650(void);
extern void func_02019ed0(void);
extern void func_02011db4(void);

extern unsigned char data_0209f1d8;
extern int data_0209fc68;
extern int data_0208ee44;
extern unsigned char data_0208ee48;
extern unsigned char data_0209d4ec;
extern unsigned char data_0209b480;
extern unsigned char data_0209b474;
extern short data_0209d504;
extern unsigned char data_0209d4f4;

void func_02019ac4(void)
{
    int skip;
    unsigned short lid;
    int delta;
    int closed;
    unsigned int t;

    if (func_0203da3c() != 0 || data_0209f1d8 != 0)
        skip = 1;
    else
        skip = 0;
    lid = *(unsigned short *)0x027fffa8;
    closed = (int)(lid & 0x8000) >> 15;
    t = data_0209fc68;
    delta = data_0208ee44;
    t = t == 6;
    if (t != false)
        data_0208ee48 = 0;
    if (data_0209d4ec == 0) {
        if (closed != 0) {
            if (data_0209b480 != 0) {
                data_0209b474 = 1;
                func_02019e60();
                data_0209d4ec = 1;
            }
        }
    }
    if (data_0209d4ec == 1) {
        short tv = data_0209d504;
        if (tv > 0)
            goto dec1;
        {
            if (data_0208ee48 != 0) {
                int arg;
                if (func_0203d974() != 0) {
                    data_0209d504 = 0x3c;
                    arg = 0x44;
                } else {
                    data_0209d504 = 0x78;
                    arg = 0x10;
                }
                func_02012790(arg);
            } else {
                data_0209d504 = 0;
            }
            data_0209d4ec = 2;
        }
        goto st2;
    dec1:
        data_0209d504 = tv - delta;
    }
st2:
    if (data_0209d4ec == 2) {
        if (closed == 0) {
            func_02019dfc();
            data_0209d4ec = 6;
        } else {
            short tv = data_0209d504;
            if (tv > 0)
                goto dec2;
            {
                if (data_0209d4f4 == 0) {
                    func_02011dc0();
                    data_0209d4f4 = 1;
                }
                if (skip != 0)
                    func_0205f66c(0);
                data_0209d4ec = 3;
            }
            goto st3;
        dec2:
            data_0209d504 = tv - delta;
        }
    }
st3:
    if (data_0209d4ec == 3) {
        if (skip == 0) {
            func_02019ed0();
            data_0209d4ec = 4;
        } else {
            if (closed == 0)
                data_0209d4ec = 4;
        }
    }
    if (data_0209d4ec == 4) {
        if (func_0205f650() == 1 || func_0205f66c(1) == 1) {
            if (data_0209d4f4 != 0) {
                func_02011db4();
                data_0209d4f4 = 0;
            }
            func_02019dfc();
            data_0209d4ec = 5;
            data_0209d504 = 1;
        }
    }
    if (data_0209d4ec == 5) {
        short tv = data_0209d504;
        if (tv > 0)
            goto dec5;
        {
            if (data_0208ee48 != 0) {
                int arg;
                if (func_0203d974() != 0) {
                    data_0209d504 = 0x3c;
                    arg = 0x45;
                } else {
                    data_0209d504 = 0xb4;
                    arg = 0x11;
                }
                func_02012790(arg);
            } else {
                data_0209d504 = 0;
            }
            data_0209d4ec = 6;
        }
        goto st6;
    dec5:
        data_0209d504 = tv - delta;
    }
st6:
    if (data_0209d4ec == 6) {
        short tv = data_0209d504;
        if (tv <= 0) {
            data_0209d4ec = 0;
            data_0209b474 = 0;
        } else {
            data_0209d504 = tv - delta;
        }
    }
}
