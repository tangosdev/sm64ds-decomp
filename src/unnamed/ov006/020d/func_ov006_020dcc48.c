/* func_ov006_020dcc48 at 0x020dcc48
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern int data_ov006_021341ec;
extern int data_ov006_0212e364[];
extern void func_ov004_020b0380(int a, int b, int c, int d);

void func_ov006_020dcc48(void) {
    int i;
    int s, v, j;
    for (i = 0; i < 3; i++) {
        v = data_ov006_0212e364[i];
        s = 0x10;
        for (j = 0; j < 0x10; j++) {
            func_ov004_020b0380(data_ov006_021341ec, s, v, 0);
            s += 0x20;
        }
    }
}
