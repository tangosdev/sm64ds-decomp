/* func_ov007_020c7804 at 0x020c7804
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern void func_ov007_020c8010(void* a, void* b, void* c);

struct Obj {
    void** field0;   // 0x0
    int count;       // 0x4
    void** field8;   // 0x8
    int field_c;     // 0xc
    int field_10;    // 0x10
};

void func_ov007_020c7804(struct Obj* self) {
    int count = self->count;
    int n;
    int i;

    if (count <= 1) {
        n = 0;
    } else {
        n = count - !(self->field_c & 2);
    }

    switch (self->field_10) {
    case 0:
    case 1:
    case 2:
    case 3:
        for (i = 0; i < n; i++) {
            int j;
            if (i < self->count - 1)
                j = i + 1;
            else
                j = 0;
            func_ov007_020c8010(self->field8[i], self->field0[i], self->field0[j]);
        }
        break;
    default:
        break;
    }
}
