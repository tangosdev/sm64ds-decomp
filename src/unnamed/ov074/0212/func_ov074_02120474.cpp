//cpp
extern "C" {
extern int _ZN9Animation7AdvanceEv(void*);
extern short data_ov074_02122e04[];
int func_ov074_02120474(char* c) {
    _ZN9Animation7AdvanceEv(c + 0x3d0);
    unsigned char idx = *(unsigned char*)(c + 0x604);
    unsigned int val = *(unsigned int*)(c + 0x3d8);
    unsigned short tbl = data_ov074_02122e04[idx];
    unsigned int v = (val << 4) >> 0x10;
    if (v >= tbl) {
        *(unsigned int*)(c + 0x3d8) = (unsigned int)tbl << 0xc;
        return 1;
    }
    return 0;
}
}
