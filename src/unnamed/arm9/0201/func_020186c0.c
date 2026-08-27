struct S82 {
    void *f0;           /* +0 */
    char pad4[4];
    unsigned short f8;  /* +8 */
    unsigned short fa;  /* +0xa */
    char padc[8];
};
extern struct S82 data_0208ecf4[];
extern unsigned char data_0208eb54;
extern int LoadArchive(int idx);
extern void Crash(void);
extern int func_0204ede8(void *o, unsigned int i);

int func_020186c0(unsigned int val)
{
    unsigned int i;
    struct S82 *e;
    if (val < 0x8000) return 0;
    e = data_0208ecf4;
    for (i = 0; i < 0xd; i++) {
        if (e->f8 > val) goto next;
        if (val >= e->fa) goto next;
        if (e->f0 == 0) {
            if (LoadArchive(i)) data_0208eb54 = (unsigned char)i;
            if (e->f0 == 0) {
                Crash();
                return 0;
            }
        }
        return func_0204ede8(e->f0, val - e->f8);
    next:
        e++;
    }
    Crash();
    return 0;
}
