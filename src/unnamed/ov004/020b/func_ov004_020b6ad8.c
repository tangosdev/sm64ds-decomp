/* func_ov004_020b6ad8 at 0x020b6ad8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
 */
extern void *data_ov004_020beb68;
extern int data_ov004_020bc850;
extern int data_ov004_020bc89c;
extern int func_ov004_020adbc0(void);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);

void func_ov004_020b6ad8(void) {
    int r;
    if (data_ov004_020beb68 == 0) return;
    r = func_ov004_020adbc0();
    func_ov004_020b2444(data_ov004_020bc850, data_ov004_020bc89c, r, -1, -1, 0, 0);
}
