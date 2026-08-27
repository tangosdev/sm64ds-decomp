/* func_ov006_020cefa4 at 0x020cefa4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern int data_ov006_02140838;
extern char *data_ov006_0214082c;
extern int func_ov006_020ceabc(char *p, int a0, int a1, int a2, int a3);

char *func_ov006_020cefa4(int a0, int a1, int a2, int a3)
{
    int i = 0;
    char *base;
    int off;
    if (data_ov006_02140838 > 0) {
        base = data_ov006_0214082c;
        do {
            if (*(int*)(base + 0x84) == 0) {
                off = i * 0x1d0;
                func_ov006_020ceabc(data_ov006_0214082c + off, a0, a1, a2, a3);
                return data_ov006_0214082c + off;
            }
            i++;
            base += 0x1d0;
        } while (i < data_ov006_02140838);
    }
    return 0;
}
