//cpp
extern "C" {
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern void func_ov006_020d5d90(char *base, int idx);
}

extern "C" void func_ov006_020d66c4(char *base, int idx) {
    char *ip = base + idx * 16;
    if (*(unsigned char*)(ip + 0x6000 + 0x26c) != 0) return;
    *(unsigned char*)(ip + 0x6000 + 0x26c) = 1;
    _ZN5Sound12PlayBank2_2DEj(0x1e2);
    if (idx != 0) return;
    func_ov006_020d5d90(base, idx);
}
