typedef enum { FALSE, TRUE } Bool;

extern void func_ov007_020c9a2c(void *buf, int x);
extern void func_ov007_020c3550(int a, int b, int c, int d);

int func_ov007_020c99d8(int x)
{
    char buf[0x84];
    void *ip;
    unsigned char a;
    unsigned char b;
    unsigned char c;
    Bool d;

    if (x == 0) {
        return 0;
    }
    func_ov007_020c9a2c(buf, x);
    ip = *(void **)(buf + 0x44);
    a = *(unsigned char *)((char *)ip + 8);
    b = *(unsigned char *)((char *)ip + 0);
    c = *(unsigned char *)((char *)ip + 4);
    if (a == 0) {
        d = FALSE;
    } else {
        d = TRUE;
    }
    func_ov007_020c3550((int)((char *)ip + 0xc), b, c, d);
}