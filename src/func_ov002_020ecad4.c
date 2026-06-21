/* func_ov002_020ecad4 at 0x020ecad4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a, int fix, unsigned int e);
extern void *data_ov002_0210eb78[];

void func_ov002_020ecad4(char *r0) {
    *(int*)(r0 + 0x98) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r0 + 0x300, data_ov002_0210eb78[1], 0, 0x1000, 0);
}
