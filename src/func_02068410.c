typedef unsigned char u8;

struct G9d2c {
    char pad[0xc];
    u8 state;   /* 0xc */
};
extern struct G9d2c data_020a9d2c;

extern int func_02068398(void);
extern void func_0206834c(void);
extern void func_020681b8(void);
extern void func_02068188(void);
extern void func_02067f68(void);

void func_02068410(void)
{
    for (;;) {
        switch (data_020a9d2c.state) {
        case 0:
        case 1:
            if (func_02068398() != 0) break;
            return;
        case 2:
            func_0206834c();
            break;
        case 3:
            func_020681b8();
            return;
        case 4:
            func_02068188();
            break;
        case 5:
            func_02067f68();
            return;
        case 6:
            break;
        }
    }
}
