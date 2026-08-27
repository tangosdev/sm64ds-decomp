extern int DecIfAbove0_Short(void*);
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned, unsigned, unsigned, int, int);
extern unsigned short data_ov100_02148708[];

int func_ov100_02145014(void) {
    int r = DecIfAbove0_Short(data_ov100_02148708);
    if (r) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x24, 0x14, 0x7f, 0x15666, 0);
    }
    return data_ov100_02148708[0] == 0 ? 1 : 0;
}
