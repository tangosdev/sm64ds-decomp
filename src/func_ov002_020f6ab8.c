/* func_ov002_020f6ab8 at 0x020f6ab8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Vector3 { int x, y, z; };
extern unsigned int ReadUnalignedInt(unsigned char *p);
extern int _ZN5Sound4PlayEjjRK7Vector3(unsigned int a, unsigned int b, const struct Vector3 *v);

int func_ov002_020f6ab8(unsigned char *p, unsigned char *q) {
    unsigned int v = ReadUnalignedInt(q);
    _ZN5Sound4PlayEjjRK7Vector3(1, v, (struct Vector3*)(p + 0x74));
    return 1;
}
