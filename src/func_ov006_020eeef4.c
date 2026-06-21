/* func_ov006_020eeef4 at 0x020eeef4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

extern void *data_ov006_021421c0;

void func_ov006_020eeef4(void **r0) {
    void **head = (void **)&data_ov006_021421c0;
    void **cur = (void **)*head;
    if (cur == r0) {
        *head = *r0;
        return;
    }
    void **next = (void **)*cur;
    if (next == 0) return;
    do {
        if (next == r0) {
            *cur = *r0;
            return;
        }
        cur = next;
        next = (void **)*next;
    } while (next != 0);
}
