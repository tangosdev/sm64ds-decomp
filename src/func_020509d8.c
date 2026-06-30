/* func_020509d8 at 0x020509d8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void *data_020a5bb8;
extern int func_0205d368(int *o, int r1, int sel);
extern int func_0205d3d4(int *o, int a1, unsigned int len);

int func_020509d8(unsigned int idx, int a1, unsigned int len, unsigned int off)
{
    char *G = (char *)data_020a5bb8;
    char *T = *(char **)(G + 0x7c);
    char *entry;
    unsigned int avail;

    if (idx >= *(unsigned int *)(T + 8))
        return -1;

    entry = T + 0xc + idx * 16;
    avail = *(unsigned int *)(entry + 4) - off;
    if (len > avail)
        len = avail;

    if (func_0205d368((int *)(G + 0x30), *(int *)entry + off, 0) == 0)
        return -1;

    return func_0205d3d4((int *)(G + 0x30), a1, len);
}
