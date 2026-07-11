extern int func_ov063_0211a3d0(char *c);
extern int func_ov063_0211a564(char *c, int arg1);

void func_ov063_02118cd8(char *self) {
    if (*(unsigned short *)(self + 0x100) == 0) {
        unsigned int v = *(unsigned short *)(self + 0x5d4);
        if ((v << 25 >> 31) == 0) {
            unsigned char *p = (unsigned char *)(((int)self + 0x5ca) & 0xFFFFFFFFFFFFFFFFLL);
            (*p)--;
        }
    }
    if (*(unsigned char *)(self + 0x5ca) == 0) {
        unsigned short *p;
        if (func_ov063_0211a3d0(self) == 0) return;
        *(int *)(((int)self + 0x19c) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        p = (unsigned short *)(((int)self + 0x5d4) & 0xFFFFFFFFFFFFFFFFLL);
        *p &= ~8;
        *(unsigned char *)(self + 0x5cc) = 4;
        *(unsigned short *)(self + 0x92) = 0;
        *(unsigned short *)(self + 0x94) = 0;
        *(unsigned short *)(self + 0x96) = 0;
        *p |= 0x80;
        return;
    }
    if (*(unsigned short *)(self + 0x100) == 0) {
        *(int *)(((int)self + 0x584) & 0xFFFFFFFFFFFFFFFFLL) -= 0x255;
    }
    if (func_ov063_0211a564(self, 0x28) != 0) {
        *(unsigned char *)(self + 0x5cc) = 1;
    }
}
