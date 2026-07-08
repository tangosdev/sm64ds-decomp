/* func_ov004_020b65e4 at 0x020b65e4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
 */
extern int data_ov004_020bf9ec;
extern int data_ov004_020bfa18;
extern unsigned char data_020a0e40;
extern unsigned char TOUCH_INPUT_ARR[][4];
extern unsigned char data_020a0de9[][4];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];
extern unsigned char data_ov004_020bfa34[];
extern void (*data_ov004_020bfa20)(void);

extern void func_ov004_020b6234(void);
extern void func_ov004_020b52fc(void *c);

void func_ov004_020b65e4(void) {
    int i;
    unsigned char *p;

    if (data_ov004_020bf9ec == 0 && data_ov004_020bfa18 != 0) {
        int idx = data_020a0e40;
        int flag = TOUCH_INPUT_ARR[idx][0] != 0 && data_020a0de9[idx][0] != 0;
        if (flag) {
            int a = data_020a0dea[idx][0];
            int b = data_020a0deb[idx][0];
            if (a < 0x40 && b < 0x50) {
                func_ov004_020b6234();
            }
        }
    }

    p = data_ov004_020bfa34;
    for (i = 0; i < 0x14; i++) {
        func_ov004_020b52fc(p);
        p += 0x24;
    }

    if (data_ov004_020bfa20 != 0) {
        data_ov004_020bfa20();
    }
}
