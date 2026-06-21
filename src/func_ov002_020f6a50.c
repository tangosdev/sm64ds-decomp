/* func_ov002_020f6a50 at 0x020f6a50
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_020072c0(void);
extern void func_0203d384(void);
extern void func_020733a8(void* self, int a, int b, void* fn, void* fn2);

void* func_ov002_020f6a50(int* self)
{
    func_020733a8(self, 1, 0xc, (void*)func_0203d384, (void*)func_020072c0);
    self[0] = 0;
    self[1] = 0;
    self[2] = 0;
    return self;
}
