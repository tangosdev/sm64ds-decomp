/* func_ov002_020f2340 at 0x020f2340
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void _ZN3OAM9RenderSubEP7OamAttrii(void *oam, int a, int b);
extern char data_ov002_0210bddc;

void func_ov002_020f2340(char *self) {
    char *p = *(char**)(self + 0xd4);
    _ZN3OAM9RenderSubEP7OamAttrii(&data_ov002_0210bddc, 0x80, *(unsigned short*)(p + 8));
    p = *(char**)(self + 0xd4);
    _ZN3OAM9RenderSubEP7OamAttrii(&data_ov002_0210bddc, 0x80, *(unsigned short*)(p + 0xa));
}
