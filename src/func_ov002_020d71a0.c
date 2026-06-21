/* func_ov002_020d71a0 at 0x020d71a0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void *data_ov002_0210eb88;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int id, int fix, unsigned int j);

void func_ov002_020d71a0(char *self)
{
    *(char *)(self + 0x714) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(*(void **)(self + 0x160), *(void **)((char *)&data_ov002_0210eb88 + 4), 0x40000000, 0x1000, 0);
    *(int *)(*(char **)(self + 0x160) + 0x58) = 0;
}
