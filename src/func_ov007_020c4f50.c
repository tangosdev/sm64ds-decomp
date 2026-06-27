/* func_ov007_020c4f50 at 0x020c4f50
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern void func_ov007_020c4fcc(int r0, int r1);
extern void func_ov007_020c19cc(int r0, int r1, int r2, int r3);

struct S {
    char pad0[8];
    int count;       /* +8 */
    char pad1[0x14]; /* up to 0x20 */
    void *a;         /* +0x20 */
    int b;           /* +0x24 */
    int *arr;        /* +0x28 */
    char pad2[0xc];  /* up to 0x38 */
    void **parr;     /* +0x38 */
};

void func_ov007_020c4f50(struct S *r7, int r6, int r2, int r3)
{
    int i;
    int *r4 = r7->arr;
    func_ov007_020c4fcc(r2, r3);
    for (i = 0; i < r7->count; i++) {
        func_ov007_020c19cc(*(unsigned short *)((char *)r7->parr[i] + 8), r4[i], r6, 0x1000);
    }
    func_ov007_020c19cc(*(unsigned short *)((char *)r7->a + 8), r7->b, r6, 0x1000);
}
