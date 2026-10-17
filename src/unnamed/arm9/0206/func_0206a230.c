#pragma optimize_for_size on

extern char *data_020a9db4;

int func_0206a230(void)
{
    char *r2;
    unsigned int r0;
    unsigned int ip;
    unsigned int r3;
    unsigned short r1;

    /* r3/ip/r0 stay materialized 0/1 flags: return A && B && C, chained inline
       predicates and ?1:0 each change the size (0x50-0x78) */
    r0 = 0;
    r2 = data_020a9db4;
    ip = 0;
    r3 = 0;
    r1 = *(unsigned short *)(r2 + 0x1e00 + 0x28);
    if (r1 != 1) { /* empty arm: r1 == 1 && ... re-colours r3 (0x74) */
    } else if (*(unsigned char *)(r2 + 0x1000 + 0x410) == 0) {
        r3 = 1;
    }
    if (r3 != 0) {
        r1 = *(unsigned short *)(r2 + 0x1e00 + 0x26);
        if (r1 == 0) {
            ip = 1;
        }
    }
    if (ip != 0) {
        r1 = *(unsigned short *)(r2 + 0x1e00 + 0x2a);
        if (r1 != 0) {
            r0 = 1;
        }
    }
    return r0;
}