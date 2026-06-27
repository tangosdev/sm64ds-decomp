/* func_ov007_020c27b4 at 0x020c27b4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern void func_ov007_020c2d44(void *a, int i);

struct S {
    int state;          // +0
    int pad;            // +4
    unsigned short count; // +8
};

void func_ov007_020c27b4(struct S *s)
{
    int i;
    switch (s->state) {
    case 0:
        for (i = 0; i < s->count; i++)
            func_ov007_020c2d44(s, i);
        break;
    case 1:
        break;
    }
}
