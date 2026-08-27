extern unsigned char data_0209d65c;
extern unsigned char *data_0209d6f4;
extern unsigned char data_0209d674;
extern unsigned char data_0209d6a8;
extern unsigned char data_0209d6b0;
extern unsigned char data_0208f174[];
extern int func_02054d88(void);
extern unsigned int _ZN2G213GetBG2CharPtrEv(void);
extern unsigned int _ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiCopy_Int(int *dst, int *src, int len);

void func_0201b100(int mode) {
    int base; int opcode; unsigned int acc; int size; int startRow;
    int r4addr; unsigned char *cursor;
    acc = data_0209d65c;
    base = func_02054d88() + 0x8000;
    startRow = 0;
    size = 0x40;
    for (;;) {
        cursor = data_0209d6f4;
        opcode = *cursor;
        switch (opcode) {
        case 0xfd: return;
        case 0xfe: data_0209d6f4 = cursor + cursor[1]; continue;
        case 0xff: return;
        }
        if (mode != 0) {
            r4addr = _ZN2G213GetBG2CharPtrEv() + (((data_0209d674 + 0x280) + (acc >> 3)) << 5);
        } else {
            r4addr = _ZN3G2S13GetBG0CharPtrEv() + (((data_0209d6a8 + 0x200) + (acc >> 3)) << 5);
        }
        if ((acc & 0xf) == 0) {
            int lo2 = opcode & 0xf; int hi2 = opcode & 0xf0; int w;
            MultiCopy_Int((int *)(base + (((lo2 << 1) + (hi2 << 2)) << 5)), (int *)r4addr, size);
            w = ((int)data_0209d6b0 + 7) / 8;
            MultiCopy_Int((int *)(base + ((lo2 + ((hi2 << 1) + 0x10)) << 6)), (int *)(r4addr + (w << 5)), size);
        } else {
            int lo; int hi; int r8addr; int sh; int r7addr; int i; int row; short rem; short val; int w8; int inv; int lshift; int rshift;
            lo = opcode & 0xf;
            hi = opcode & 0xf0;
            r8addr = base + (((lo << 1) + (hi << 2)) << 5);
            sh = acc & 7;
            val = (short)(int)data_0208f174[opcode];
            row = startRow;
            w8 = (((int)data_0209d6b0 + 7) / 8) << 3;
            r7addr = r8addr + 0x20;
            lshift = sh << 2;
            inv = 8 - sh;
            rshift = inv << 2;
            for (; row < 0x10; row++) {
                i = (row >> 3) * w8 + (row & 7);
                ((int *)r4addr)[i] |= *(int *)r8addr << lshift;
                rem = val;
                rem -= inv;
                if (rem > 0) {
                    (((int *)r4addr) + i)[8] = (*(unsigned int *)r8addr >> rshift) | (*(int *)r7addr << lshift);
                    rem = rem - 8;
                }
                if (rem > 0)
                    *(int *)((char *)r4addr + (i << 2) + 0x40) = *(unsigned int *)r7addr >> rshift;
                r8addr += 4;
                r7addr += 4;
                if (row == 7) { r8addr += 0x3e0; r7addr += 0x3e0; }
            }
        }
        acc = (acc + data_0208f174[opcode]) & 0xff;
        data_0209d6f4 = data_0209d6f4 + 1;
    }
}
