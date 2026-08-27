extern void func_02012718(int a, void *b);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int x);

void func_ov006_020dedfc(char *r7, int r6, int r5, int r4)
{
    if (r6 == 6) {
        if (r5 == 4) {
            func_02012718(0x1cf, *(void **)(r7 + (r4 << 3) + 0x5000 + 0x3e8));
        } else if (r5 == 5 || r5 == 0xb) {
            func_02012718(0x1d0, *(void **)(r7 + (r4 << 3) + 0x5000 + 0x3e8));
        }
    }
    if (r6 == 5 && r5 == 6 && r4 == *(unsigned char *)(r7 + 0x5000 + 0x46d)) {
        func_02012718(0x1ce, *(void **)(r7 + (r4 << 3) + 0x5000 + 0x3e8));
    }
    if (r6 != 1 && r6 != 4) return;
    if (r5 != 4) return;
    _ZN5Sound12PlayBank2_2DEj(0x1cc);
}
