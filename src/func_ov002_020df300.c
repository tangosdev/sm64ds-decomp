/* func_ov002_020df300 at 0x020df300
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Camera;
extern void *data_0209f318;
extern void func_0200d438(void *thiz, unsigned char playerID);

int func_ov002_020df300(unsigned char *self)
{
    if (self[0x6e3] != 0)
        return 0;
    self[0x6e3] = 1;
    func_0200d438(*(void **)&data_0209f318, self[0x6d8]);
    return 1;
}
