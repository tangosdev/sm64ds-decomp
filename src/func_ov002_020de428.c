/* func_ov002_020de428 at 0x020de428
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Vector3 { int x, y, z; };
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int a, const struct Vector3 *v);

void func_ov002_020de428(unsigned char *p) {
    if (p[0x70c] != 0) return;
    _ZN5Sound9PlayBank0EjRK7Vector3(0xbd, (struct Vector3*)(p + 0x74));
    p[0x70c] = 0x1e;
}
