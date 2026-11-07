/* storage 0x02385FC0 runtime 0x037FDE58 size 0x48 wram. One literal-pool data ref (unk_3807B04), no calls. */

extern char unk_3807B04[];

int fn_02385FC0(int a1, int a2, int a3, int a4) {
    char *v4 = unk_3807B04 + 36 * a1;
    switch (a4) {
    case 1:
        v4[a2] = a3;
        break;
    case 2:
        *(short *)&v4[a2] = a3;
        break;
    case 4:
        *(int *)&v4[a2] = a3;
        break;
    }
    return a1;
}
