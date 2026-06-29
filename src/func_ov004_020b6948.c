/* func_ov004_020b6948 at 0x020b6948
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
 */
extern void *data_ov004_020beb68;
extern int data_ov004_020bc85c;
extern int data_ov004_020bc870;
extern unsigned char *data_ov004_020bca44[];
extern int func_ov004_020adbc0(void);
extern int func_ov004_020ad674(void);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern void func_ov004_020af948(void *a, int b, int c, int d);

void func_ov004_020b6948(void) {
    unsigned int r;
    int idx;

    if (data_ov004_020beb68 == 0) return;

    r = func_ov004_020adbc0();
    func_ov004_020b2444(data_ov004_020bc85c + 4, data_ov004_020bc870, r / 100, -1, -1, 1, 0);

    r = func_ov004_020adbc0();
    func_ov004_020b2444(data_ov004_020bc85c + 0x1c, data_ov004_020bc870, r % 100 / 10, -1, -1, 1, 0);

    r = func_ov004_020adbc0();
    func_ov004_020b2444(data_ov004_020bc85c + 0x2c, data_ov004_020bc870, r % 10, -1, -1, 1, 0);

    idx = func_ov004_020ad674();
    func_ov004_020af948((void *)*(int *)(data_ov004_020bca44[idx] + 0xc), data_ov004_020bc85c - 0x24, data_ov004_020bc870, 0);

    idx = func_ov004_020ad674();
    func_ov004_020af948((void *)*(int *)(data_ov004_020bca44[idx] + 0x1c), data_ov004_020bc85c + 0x10, data_ov004_020bc870, 0);
}
