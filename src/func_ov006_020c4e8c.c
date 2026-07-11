#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))
int func_ov006_020e6e3c(int a, int b);
void func_ov006_020c4d3c(char* c);
extern short data_02082214[];

void func_ov006_020c4e8c(char* c) {
    if (*(short*)(c + 0xea) < 0x4000) {
        short* p = (short*)AT(c, 0xea);
        short* tbl = data_02082214;
        int ang, idx, idx1, r0;
        *p = *p + 0x400;
        ang = *(unsigned short*)(c + 0xea) >> 4;
        idx = ang * 2;
        idx1 = idx + 1;
        r0 = (int)(((long long)((tbl[idx] >> 1) + 0x1000) * (long long)0x1400 + 0x800) >> 0xc);
        *(int*)(c + 0xcc) = r0;
        *(int*)(c + 0xd0) = (int)(((long long)((tbl[idx1] >> 1) + 0x800) * (long long)0x1400 + 0x800) >> 0xc);
        *(int*)(c + 0xd4) = r0;
        return;
    }
    func_ov006_020e6e3c(0x1c7, *(int*)(c + 0x9c));
    func_ov006_020c4d3c(c);
}
