// NONMATCHING: different op / idiom (div=33). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern short data_ov006_0212edb8[];
extern short data_ov006_0212edba[];
extern short data_ov006_0212ed88[];
extern short data_ov006_0212ed8a[];
extern short data_ov006_0212ed8c[];
extern short data_ov006_0212ed8e[];

int func_ov006_02108b90(short *r0, int r1)
{
    int r3 = *(short *)((char *)r0 + 0x2c);
    int r2 = 0;
    int ip;
    ip = (r3 <= 0xb) ? 1 : 0;
    if (ip != 0) {
        if (r3 == r1) r2 = 0xc;
    } else if (r3 >= 0xc) {
        ip = (r3 <= 0x1c) ? 1 : 0;
        if (ip != 0) {
            ip = (r3 - 0xc) << 2;
            if (r1 == *(short *)((char *)data_ov006_0212edb8 + ip) ||
                r1 == *(short *)((char *)data_ov006_0212edba + ip))
                r2 = 6;
        } else if (r3 >= 0x1d) {
            ip = (r3 <= 0x22) ? 1 : 0;
            if (ip != 0) {
                ip = (r3 - 0x1d) << 3;
                if (r1 == *(short *)((char *)data_ov006_0212ed88 + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8a + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8c + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8e + ip))
                    r2 = 3;
            } else if (r3 == 0x23) {
                if (r1 >= 6) r2 = 2;
            } else if (r3 == 0x24) {
                if (r1 >= 6) r2 = 2;
            }
        }
    }
    *(short *)((char *)r0 + 0x2e) = r2;
    return r2;
}
