/* func_ov006_02120b7c at 0x02120b7c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Node { struct Node *next; };
extern struct Node *data_ov006_02142f64;

struct Node *func_ov006_02120b7c(struct Node *r0) {
    struct Node *r2 = data_ov006_02142f64;
    if (r2 == r0) {
        r0 = r0->next;
        data_ov006_02142f64 = r0;
        return r0;
    }
    struct Node *r1 = r2->next;
    if (r1 == 0) return r0;
    do {
        if (r1 == r0) {
            r0 = r0->next;
            r2->next = r0;
            return r0;
        }
        r2 = r1;
        r1 = r1->next;
    } while (r1 != 0);
    return r0;
}
