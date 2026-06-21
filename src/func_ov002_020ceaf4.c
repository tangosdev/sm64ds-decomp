/* func_ov002_020ceaf4 at 0x020ceaf4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int data_0209f32c;
extern int data_ov002_020ff1e0[];

int func_ov002_020ceaf4(char *self) {
    int v = data_0209f32c - 0x50000 - *(int *)(self + 0x60);
    if (v < 0x1f4000)
        return 0x1800;
    int val = data_ov002_020ff1e0[*(int *)(self + 8)];
    return -(int)((((long long)val << 13) + 0x800) >> 12);
}
