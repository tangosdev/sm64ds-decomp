// NONMATCHING: register allocation (div=48). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

void func_ov006_020dc7fc(char *p);
void func_ov006_020dc334(char *p);

void func_ov006_020ddf9c(char *c) {
    int i;
    int j;
    int k;
    int m;
    char *p;

    p = c;
    for (i = 0; i < 0x28; i++) {
        *(u32*)(p + 0x4660) = 0;
        *(u32*)(p + 0x4664) = 0;
        *(u32*)(p + 0x4668) = 0;
        *(u32*)(p + 0x466c) = 0;
        *(u16*)(p + 0x4670) = 0;
        *(u16*)(p + 0x4672) = 0;
        *(u8*)(p + 0x4674) = 0;
        *(u8*)(p + 0x4675) = 0;
        *(u8*)(p + 0x4676) = 0;
        *(u8*)(p + 0x4677) = 0;
        *(u8*)(p + 0x4678) = 0;
        *(u8*)(p + 0x4679) = 0;
        *(u8*)(p + 0x467a) = 0;
        *(u8*)(p + 0x467b) = 0;
        p += 0x1c;
    }
    p = c;
    for (j = 0; j < 0x18; j++) {
        *(u32*)(p + 0x4ac0) = 0;
        *(u32*)(p + 0x4ac4) = 0;
        *(u8*)(p + 0x4ad0) = 0;
        *(u8*)(p + 0x4ad1) = 0;
        *(u8*)(p + 0x4ad2) = 0;
        *(u8*)(p + 0x4ad3) = 0;
        *(u8*)(p + 0x4ad4) = 0;
        *(u8*)(p + 0x4ad5) = 0;
        p += 0x18;
    }
    *(u32*)(c + 0x4d00) = 0;
    *(u32*)(c + 0x4d04) = 0;
    *(u16*)(c + 0x4d08) = 0;
    *(u16*)(c + 0x4d0a) = 0;
    *(u16*)(c + 0x4d0c) = 0;
    *(u8*)(c + 0x4d13) = 0;
    p = c;
    for (k = 0; k < 0x20; k++) {
        *(u8*)(p + 0x4d28) = 0;
        *(u8*)(p + 0x4d29) = 0;
        p += 0x18;
    }
    for (m = 0; m < 0x18; m++) {
        char *e = c + (m << 4);
        *(u8*)(e + 0x5020) = 0;
        *(u8*)(e + 0x5021) = 0;
    }
    *(u32*)(c + 0x51c8) = 0;
    *(u32*)(c + 0x51cc) = 0;
    *(u8*)(c + 0x51db) = 0;
    *(u8*)(c + 0x51dd) = 0;
    *(u32*)(c + 0x51d0) = 0;
    *(u8*)(c + 0x51de) = 0;
    func_ov006_020dc7fc(c);
    func_ov006_020dc334(c);
    *(u8*)(c + 0x51df) = 0;
}
