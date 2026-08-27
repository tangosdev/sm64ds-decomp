/* func_ov007_020b45b0 at 0x020b45b0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
struct DataEntry { int flags; int b; int c; };
extern struct DataEntry data_ov007_020d77dc[];
extern int func_ov007_020b79e4(void);
extern int func_ov007_020b63e4(void *t);

void func_ov007_020b45b0(void *t) {
    char *p = *(char**)t;
    int call_it = 0;
    int t0 = 0;
    int b6 = 0;

    if (*(*(short**)(p + 4)) == 0) {
        if (*(int*)(p + 0x10) >= 0x1000 ||
            (data_ov007_020d77dc[**(unsigned short**)t].flags & 2))
            b6 = 1;
    }
    if (b6) {
        if (!(data_ov007_020d77dc[**(unsigned short**)t].flags & 1))
            t0 = 1;
    }
    if (t0) {
        int ok = 1;
        if (func_ov007_020b79e4()) {
            int special = 0;
            if (func_ov007_020b79e4()) {
                unsigned short id2 = **(unsigned short**)t;
                int x = ok;
                if (id2 != 0x16 && id2 != 0x17) x = special;
                if (x) special = 1;
            }
            if (!special) ok = 0;
        }
        if (ok) call_it = 1;
    }
    if (!call_it) return;
    func_ov007_020b63e4(t);
}
