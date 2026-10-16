/* storage 0x0239E784 runtime 0x027D06FC main. */

extern void fn_02395354(unsigned int addr, unsigned int val);

typedef struct {
    unsigned char skip[0x300];
    unsigned char raw[0x5C];
    unsigned short lo;
    unsigned short hi;
} FlagBlk;

#pragma scheduling off
#pragma opt_propagation off
int fn_0239E784(int a1) {
    int count;
    unsigned int i;
    int one;
    char **pp;
    unsigned short *table;
    int bit;
    FlagBlk *blk;
    unsigned int mask;

    count = 0;
    fn_02395354((unsigned int)a1, 1);
    count += 2;
    i = 0;
    table = (unsigned short *)0x027D6514;
    one = 1;
    pp = (char **)0x0380FFF4;
    do {
        bit = one << i;
        blk = (FlagBlk *)*pp;
        mask = blk->hi;
        mask &= (unsigned int)bit;
        if (mask != 0) {
            unsigned int sel = blk->lo;

            sel &= (unsigned int)bit;
            if (sel != 0)
                fn_02395354((unsigned int)(a1 + count), (unsigned char)(table[i] | 0x80));
            else
                fn_02395354((unsigned int)(a1 + count), (unsigned char)table[i]);
            count += 1;
        }
        i += 1;
    } while (i < 0x10);
    fn_02395354((unsigned int)(a1 + 1), (unsigned char)(count - 2));
    return count;
}
