/* func_ov002_020cd364 at 0x020cd364
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_ov002_020dc020(void *self);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);

void func_ov002_020cd364(char *self) {
    func_ov002_020dc020(self);
    _ZN12CylinderClsn5ClearEv(self + 0x314);
    _ZN12CylinderClsn6UpdateEv(self + 0x314);
    *(short *)(self + 0x6a4) = 2;
    *(unsigned char *)(self + 0x6e3) = 5;
}
