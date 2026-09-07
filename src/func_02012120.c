/* func_02012120 at 0x02012120
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

typedef struct { int x, y, z; } Vector3;

extern unsigned char data_02075250[];

extern void _ZN5Sound8PlayLongEjjjRK7Vector3s(
    unsigned int a, unsigned int b, unsigned int c,
    const Vector3 *v, unsigned int e);

void func_02012120(unsigned int p0, unsigned int p1, unsigned int p2,
                   const Vector3 *p3, short p4)
{
    p2 += data_02075250[p1];
    _ZN5Sound8PlayLongEjjjRK7Vector3s(p0, 1, p2, p3, (unsigned int)p4);
}
