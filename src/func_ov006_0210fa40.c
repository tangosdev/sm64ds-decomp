/* func_ov006_0210fa40 at 0x0210fa40
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct E { int v; int pad; };

int func_ov006_0210fa40(char *r0) {
    int i = 0;
    do {
        if (((struct E *)(r0 + 0x50))[i].v > 0) {
            return 0;
        }
        i++;
    } while (i < 3);
    return 1;
}
