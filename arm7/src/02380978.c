/* storage 0x02380978 runtime 0x037F8810 size 0x9C (+0xC pool) wram ARM.
 * Walk 25 bits of a1; for each set bit pick a target slot by index range
 * (8..11 and 3..6 index into unk_3806DE4, 0 takes off_3806E44, anything
 * else drops into jpt_37F8484) and fill it. */

extern int jpt_37F8484[];
extern char off_3806E44[];
extern char unk_3806DE4[];

unsigned int fn_02380978(unsigned int a1, int a2) {
    int v2 = 0;
    char *v6 = 0;
    do {
        if (a1 & 1) {
            v6 = 0;
            if (v2 >= 8 && v2 <= 11) {
                v6 = &unk_3806DE4[12 * (v2 - 8)];
            } else if (v2 >= 3 && v2 <= 6) {
                v6 = &unk_3806DE4[12 * (v2 + 1)];
            } else if (v2 == 0) {
                v6 = off_3806E44;
            } else {
                jpt_37F8484[v2] = a2;
            }
            if (v6 != 0) {
                *(int *)v6 = a2;
                ((int *)v6)[2] = 0;
                ((int *)v6)[1] = 1;
            }
        }
        a1 >>= 1;
        ++v2;
    } while (v2 < 25);
    return a1;
}
