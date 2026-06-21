/* func_ov002_020f6ae4 at 0x020f6ae4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Vector3;
extern unsigned int ReadUnalignedInt(unsigned char *p);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, struct Vector3 *v, unsigned int d);

int func_ov002_020f6ae4(char *r0, unsigned char *r1) {
    char *r4 = r0;
    unsigned int v = ReadUnalignedInt(r1);
    *(unsigned int*)(r4 + 0xe4) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(unsigned int*)(r4 + 0xe4), 3, v, (struct Vector3*)(r4 + 0x74), 0);
    return 1;
}
