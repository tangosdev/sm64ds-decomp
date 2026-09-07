/* func_ov002_020b19dc at 0x020b19dc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void func_ov002_020b16c4(char *self);
extern void func_ov002_020b1674(char *self, char *p);
extern void func_ov002_020b1884(char *self);

int func_ov002_020b19dc(char *self)
{
    unsigned int id = *(unsigned int *)(self + 0x19c);
    if (id != 0) {
        char *p = (char *)_ZN8dActor_c10FindWithIDEj(id);
        if (p != 0) {
            if (*(int *)(self + 0x198) & 0x400000) {
                *(unsigned short *)(self + 0x3a8) = 0;
                if (*(int *)(self + 0x3a0) == 1)
                    func_ov002_020b16c4(self);
                else if (*(int *)(self + 0x3a0) == 2)
                    func_ov002_020b1674(self, p);
                else
                    func_ov002_020b1884(self);
                return 1;
            }
        }
    }
    return 0;
}
