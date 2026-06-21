/* func_ov002_020bda48 at 0x020bda48
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_ov002_020e032c(void *r0);
extern void func_ov002_020bdef0(char *c);
extern void func_ov002_020bdd9c(void *r0);
extern void func_ov002_020d80d0(char *r0);
extern void func_ov002_020bdd2c(void *r0);
extern void func_ov002_020cc1f4(void *p);
extern void func_ov002_020de968(void *r0);

void func_ov002_020bda48(char *self)
{
    *(short *)(self + 0x600 + 0xae) = 0;
    func_ov002_020e032c(self);
    func_ov002_020bdef0(self);
    func_ov002_020bdd9c(self);
    func_ov002_020d80d0(self);
    func_ov002_020bdd2c(self);
    func_ov002_020cc1f4(self);
    func_ov002_020de968(self);
}
