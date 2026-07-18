typedef unsigned char u8;
typedef unsigned short u16;

extern int func_0203da9c(void);
extern u8 func_02020168(void);
extern void func_02020304(void);
extern void func_020200e0(void);
extern void func_ov075_0211a148(char* c, void* a, int b);
extern void func_02012790(int a);
extern int func_0203d9b4(void);
extern void func_02020334(void);
extern int func_ov075_02116d40(void* c);
extern void func_02020124(void);
extern void func_02020768(void* p);

extern char data_ov075_0211d810[];
extern u8 data_0209fc5c[];
extern u16 data_020a0e58[];
extern u8 data_020a0de8[];
extern int data_0208ee44;
extern u8 data_0209b2f0[];

void func_ov075_0211867c(void* self)
{
    char* c = (char*)self;
    int id = func_0203da9c();

    if (func_02020168() != 0) {
        func_02020304();
        func_020200e0();
        func_ov075_0211a148(c, data_ov075_0211d810, 5);
        func_02012790(0x124);
    } else {
        if (func_0203d9b4() != 0) {
            if (*(int*)(c + 0x274) == 0) {
                int r3 = 0;
                int i = 0;
                u8* g = data_0209fc5c;
                for (; i < 4; i++, g++) {
                    if (*g != 0) {
                        u16* hw = (u16*)((char*)data_020a0e58 + (i << 2));
                        int bits = hw[1] & 0xc;
                        r3 |= bits;
                        if (data_020a0de8[i << 2] != 0) {
                            int b = 0;
                            if (data_020a0de8[(i << 2) + 1] != 0)
                                b = 1;
                            r3 |= b;
                        } else {
                            r3 |= 0;
                        }
                    }
                }
                {
                    int *q = (int*)(c + 0x264);
                    *q = *q - data_0208ee44;
                }
                {
                    int t = *(int*)(c + 0x264);
                    if (t <= 0)
                        goto do_stop;
                    if (t >= 0x21c)
                        goto after_timer;
                    if (r3 == 0)
                        goto after_timer;
                do_stop:
                    func_02020334();
                    *(int*)(c + 0x274) = 1;
                }
            } else {
                if (func_ov075_02116d40(c) != 0)
                    func_02020124();
            }
        } else {
            if (*(int*)(c + 0x274) == 0) {
                {
                    int *q = (int*)(c + 0x264);
                    *q = *q - data_0208ee44;
                }
                if (*(int*)(c + 0x264) < 0x1e0) {
                    func_02020334();
                    *(int*)(c + 0x274) = 1;
                }
            }
        }
    after_timer:
        {
            int r = func_0203da9c();
            u16 v = *(u16*)((char*)data_020a0e58 + (r << 2));
            if ((v & ~0xc) != 0)
                func_02012790(0xe);
        }
    }

    func_02020768(c + 0x1b4);
    func_02020768(c + 0x1e0);
    func_02020768(c + 0x20c);
    {
        u8 *bp = data_0209b2f0 + id;
        if (*bp == 0)
            func_02020768(c + 0x238);
    }

    if (*(u8*)(c + 0x1d9) != 0)
        return;
    if (*(u8*)(c + 0x205) != 0)
        return;

    *(u8*)(c + 0x1da) = 0;
    *(u8*)(c + 0x205) = 1;
    *(u8*)(c + 0x206) = 1;
    if (*(u8*)(data_0209b2f0 + id) == 0) {
        *(u8*)(c + 0x231) = 1;
        *(u8*)(c + 0x232) = 1;
        *(u8*)(c + 0x25d) = 1;
        *(u8*)(c + 0x25e) = 1;
    }
}
