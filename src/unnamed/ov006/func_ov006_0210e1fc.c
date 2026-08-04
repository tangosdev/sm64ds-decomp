extern long long func_0203d5bc(int* p);
extern void func_ov006_0210fb04(void* p);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

int func_ov006_0210e1fc(char* self, int* p) {
    int d[2];
    d[0] = p[0];
    d[1] = p[1];
    d[0] -= *(int*)(self + 8);
    d[1] -= *(int*)(self + 0xc);
    if (*(unsigned char*)(self + 0x31)) {
        if (p[1] >= *(int*)(self + 0xc) + 0x8000) return 0;
        d[1] -= 0x8000;
        return func_0203d5bc(d) < 0x40000;
    }
    if (p[1] > *(int*)(self + 0xc)) {
        if (p[0] >= *(int*)(self + 8) - 0x8000 && p[0] < *(int*)(self + 8) + 0x8000
            && p[1] < *(int*)(self + 0xc) + 0xa000) return 1;
        return 0;
    }
    if (func_0203d5bc(d) >= 0x40000) return 0;
    if (p[0] >= *(int*)(self + 8) - 0x4000 && p[0] < *(int*)(self + 8) + 0x4000) {
        func_ov006_0210fb04(*(void**)(*(char**)(self + 4) + 0x4778));
        *(unsigned char*)(self + 0x31) = 1;
        *(int*)(self + 0x34) = 0xa;
        _ZN5Sound12PlayBank2_2DEj(0x1a4);
    }
    return 1;
}
