/* func_ov002_020d4d88 at 0x020d4d88
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern int _Z14ApproachLinearRiii(int *, int, int);
extern int func_ov002_020c04e0(void *);

int func_ov002_020d4d88(char *this, int a, int b, int c)
{
    _Z14ApproachLinearRiii((int *)(this + 0x98), a, b);
    if (a != 0 || (unsigned short)(*(unsigned short *)(this + 0x600 + 0xce) & 1)) {
        func_ov002_020c04e0(this);
        return 1;
    }
    return 0;
}
