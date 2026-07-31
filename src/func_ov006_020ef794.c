/* func_ov006_020ef794 at 0x020ef794 (ov006), size 0x64
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
typedef unsigned char u8;
typedef short s16;

extern void func_ov006_020c7490(void);
extern void func_ov006_020ef768(char *self);
extern void func_ov006_020c42bc(void);

void func_ov006_020ef794(char *self)
{
    s16 *p = (s16 *)(((int)self + 0x5a74));
    *p = *p - 1;
    if (*(s16 *)(self + 0x5a74) == 0) {
        if (*(u8 *)(self + 0xc4) == 0) {
            *(u8 *)(self + 0xc3) = 1;
            *(u8 *)(self + 0xc4) = 1;
            *(s16 *)(self + 0xc0) = 0;
        }
        func_ov006_020c7490();
        func_ov006_020ef768(self);
    }
    func_ov006_020c42bc();
}