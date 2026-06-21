/* func_ov002_020d674c at 0x020d674c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Obj;
struct Vtbl { char pad[0x48]; int (*f48)(struct Obj*); };
struct Obj { struct Vtbl* vt; };

int func_ov002_020d674c(unsigned char* p) {
    struct Obj* o = *(struct Obj**)(p + 0x360);
    if (o == 0) {
        return 0;
    } else {
        return o->vt->f48(o) == 6;
    }
}
