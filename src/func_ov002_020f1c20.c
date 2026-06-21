/* func_ov002_020f1c20 at 0x020f1c20
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern signed char data_ov002_02110af0;
extern signed char data_02092120;
extern int data_ov002_02110af8;
int func_ov002_020f1c20(void) {
    if (data_ov002_02110af0 == data_02092120) {
        return data_ov002_02110af8;
    }
    return 0;
}
