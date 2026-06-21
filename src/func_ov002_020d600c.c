/* func_ov002_020d600c at 0x020d600c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int _ZN6Player7IsStateERNS_5StateE(void *self, void *st);
extern char data_ov002_02110064;

int func_ov002_020d600c(char *self) {
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110064)) {
        if (*(unsigned char*)(self + 0x6e3) == 1) {
            return 1;
        }
    }
    return 0;
}
