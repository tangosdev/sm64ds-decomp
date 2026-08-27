//cpp
// func_ov002_020e73ac at 0x020e73ac
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int func_ov002_020e9630(char*);
}

extern unsigned char data_0209f2d8;

extern "C" int func_ov002_020e73ac(char* arg) {
    int c, a, b;
    unsigned char r2 = *(unsigned char*)(arg + 0x49d);
    if (r2 == 8) {
        return 3;
    }

    a = data_0209f2d8 == 1;
    if (a != false) {
        goto ret2;
    }
    b = *(unsigned short*)(arg + 0xc) == 0xb3;
    if (b != false) {
        goto ret2;
    }
    c = *(int*)(arg + 0x444);
    if (c == 9) {
ret2:
        return 2;
    }

    if (r2 == 0) {
        goto ret1;
    }
    if (func_ov002_020e9630(arg) == 0) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}