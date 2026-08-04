/* func_ov006_0211eb90 at 0x0211eb90
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern void func_ov006_0211f454(char *c, int i);
extern void func_ov006_0211f34c(char *c, int i);

void func_ov006_0211eb90(char *c, int i) {
    if (*(unsigned char *)(c + i * 0x24 + 0x467f) == 0) {
        return;
    }
    func_ov006_0211f454(c, i);
    func_ov006_0211f34c(c, i);
}
