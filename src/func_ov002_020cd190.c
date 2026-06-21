/* func_ov002_020cd190 at 0x020cd190
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int data_0209f32c;
extern void func_ov002_020ce8bc(char *self, int v);

void func_ov002_020cd190(char *self) {
    if (*(unsigned char *)(self + 0x703) == 0) return;
    if (*(int *)(self + 0x60) + 0x180000 <= data_0209f32c) return;
    func_ov002_020ce8bc(self, *(int *)(self + 0x98));
}
