/* storage 0x023A3848 runtime 0x027D57C0 size 0x1F4 main. */

#pragma opt_propagation off

extern int fn_0239AD50(unsigned short *a1);
extern int fn_0239E91C(unsigned short *a1, int a2);
extern int fn_0239F778(int a1);
extern void fn_0239A8F8(unsigned int a1, unsigned int a2);
extern int fn_02395354(unsigned int addr, unsigned int val);
extern int fn_02382C98(unsigned short *a1, unsigned short *a2, unsigned int a3);

int fn_023A3848(unsigned short *a1) {
    unsigned char *base;
    unsigned short *v3;
    unsigned short *v4;
    int result;
    unsigned short h14;
    unsigned short *row;
    unsigned short h13;
    unsigned short h12;
    int left;

    base = *(unsigned char **)0x0380FFF4;
    v3 = (unsigned short *)(base + 996);
    v4 = (unsigned short *)(*(int *)(base + 1140) + 16);
    result = *(unsigned short *)((char *)v3 + 0x3C);
    if (result == 0)
        return result;
    h14 = a1[14];
    row = (unsigned short *)(base + 0x300);
    if (h14 != *(unsigned short *)((char *)row + 0x64))
        return (int)row;
    h13 = a1[13];
    if (h13 != *(unsigned short *)((char *)row + 0x62))
        return (int)row;
    h12 = a1[12];
    result = *(unsigned short *)((char *)row + 0x60);
    if (h12 != (unsigned short)result)
        return result;
    left = (unsigned short)a1[8] - 24;
    result = (unsigned short)v4[3] - 8;
    if (left > result)
        return result;
    result = fn_0239AD50(a1 + 15);
    if (result != 255 && result != 0) {
        int scale = 28 * result;
        if (*(unsigned short *)(*(int *)(*(int *)0x0380FFF4 + 736) + scale) != 64) {
            result = fn_0239E91C(a1 + 15, 7);
            if (result != 0)
                return fn_0239F778(result);
        } else {
            unsigned short id = (unsigned short)result;
            unsigned int bit = ((unsigned)a1[10] << 19) >> 31;
            unsigned char *slot;
            unsigned short *walk;
            unsigned int i;
            fn_0239A8F8(id, bit);
            slot = *(unsigned char **)(*(int *)0x0380FFF4 + 736) + scale;
            *(unsigned short *)(slot + 24) = *(unsigned short *)(slot + 26);
            result = (unsigned short)(1
                << *(unsigned short *)(*(unsigned char **)(*(int *)0x0380FFF4 + 736) + scale + 2));
            if ((result & v3[77]) == 0 && (result & v3[76]) != 0) {
                unsigned short *flag = v3 + 77;
                unsigned short s9;
                unsigned short s7;
                *flag = (unsigned short)(*flag | result);
                *v4 = (unsigned short)(*v4 & ~result);
                walk = v4 + 5;
                for (i = (unsigned)(result << 15) >> 16; i != 1; i = (unsigned)(i << 15) >> 16) {
                    if ((i & v3[76]) != 0)
                        walk = (unsigned short *)((char *)walk + v4[3]);
                    }
                *walk = (unsigned short)(a1[8] - 24);
                {
                    unsigned int addr = (unsigned int)((char *)walk + 3);
                    unsigned int v = a1[9] & 0xFFu;
                    s9 = (unsigned short)v;
                    fn_02395354(addr, (unsigned char)s9);
                }
                {
                    unsigned int addr = (unsigned int)(walk + 1);
                    unsigned int v = a1[7] & 0xFFu;
                    s7 = (unsigned short)v;
                    result = fn_02395354(addr, (unsigned char)s7);
                }
                if (*walk != 0)
                    return fn_02382C98(a1 + 22, walk + 4, (unsigned short)*walk + 1);
            }
        }
    }
    return result;
}
