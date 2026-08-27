/* func_ov100_02145e10 at 0x02145e10
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov100).
 */
extern int _ZN6Player16TryEnterStarDoorER7Vector3s(void *player, void *vec, short s);
extern void func_ov100_02145f68(void *c, void *p, void *a2);
extern void SetTouchScreenDelay(void);
extern unsigned char data_ov100_02148954[];

void func_ov100_02145e10(char *self, void *player)
{
    int a;
    int z = *(int*)(self + 0x88);
    if (z < 0) a = *(short*)(self + 0x8e);
    else a = *(short*)(self + 0x8e) + 0x8000;
    if (_ZN6Player16TryEnterStarDoorER7Vector3s(player, self + 0x5c, a))
        func_ov100_02145f68(self, data_ov100_02148954, player);
    SetTouchScreenDelay();
}