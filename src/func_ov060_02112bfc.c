void* _ZN5Actor10FindWithIDEj(unsigned int id);
void func_ov060_02112d48(char* c, int arg);
void func_ov060_02117a3c(char* self);
extern short data_ov060_02119294[];
extern short data_ov060_02119296[];
extern short data_ov060_02119298[];
#pragma opt_common_subs off
#pragma opt_lifetimes off
void func_ov060_02112bfc(char* c){
    char* found; int i; int r6; short* p;
    *(int*)(c + 0xa8) = *(int*)(c + 0xa0);
    found = (char*)_ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x3ac));
    if (found == 0) { *(int*)(c + 0x40c) = 0; return; }
    p = data_ov060_02119294;
    i = 0;
    r6 = 1;
    while (p[2] != 0) {
        int r1 = *(unsigned short*)(c + 0x3fc);
        if (r1 < p[2]) {
            int off = i * 6;
            short a = *(short*)((char*)data_ov060_02119294 + off);
            short b = *(short*)((char*)data_ov060_02119296 + off);
            if (a > 0) {
                r6 = (short)(b * (*(short*)((char*)data_ov060_02119298 + off) - 1 - r1));
            } else {
                i -= 1; off = i * 6;
                r6 = (short)(b * (r1 - *(short*)((char*)data_ov060_02119298 + off)));
            }
            func_ov060_02112d48(c, r6);
            if (r6 != 0 && (*(unsigned short*)(c + 0x3fc) & 1)) { func_ov060_02117a3c(found); }
            r6 = 0; break;
        }
        p += 3;
        i += 1;
    }
    if (r6 != 0) {
        short* q = (short*)(((int)found + 0x8c) & 0xFFFFFFFFFFFFFFFF);
        *(int*)(c + 0x40c) = 0;
        *(short*)(found + 0x31e) = 0;
        *(short*)(found + 0x320) = 0;
        *(short*)(found + 0x322) = 0;
        q[0] = 0; q[1] = 0; q[2] = 0;
        *(int*)(c + 0xa8) = 0;
    }
}
