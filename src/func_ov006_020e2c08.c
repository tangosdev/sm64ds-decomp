/* func_ov006_020e2c08 at 0x020e2c08 (ov006)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern void func_ov006_020e1dc8(char* self, int idx);
extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);

void func_ov006_020e2c08(char* self, int idx)
{
    int n, v, w, vol;

    if (*(u8*)(self + 0x4ee4) != 1) return;

    n = idx * 0x2c;
    v = (*(int*)(self + 0x4eb0) - *(int*)(self + 0x4660 + n)) >> 12;
    w = (*(int*)(self + 0x4eb4) - *(int*)(self + n + 0x4664)) >> 12;
    if (v < -0x2e) return;
    if (v > 0x2e) return;
    if (w < -0x14) return;
    if (w > 0x14) return;

    *(u8*)(self + n + 0x4688) = 1;
    *(u16*)(self + 0x4686 + n) = *(u16*)(self + 0x4ede);
    *(int*)(self + n + 0x4668) = *(int*)(self + 0x4ec8);

    if (*(u16*)(self + 0x4ede) < 0x9800u || *(u16*)(self + 0x4ede) > 0xe800u) {
        if (*(u16*)(self + 0x4ede) >= 0x4000u && *(u16*)(self + 0x4ede) <= 0x9800u) {
            *(u16*)(self + 0x4ede) = 0x9800;
        } else {
            *(u16*)(self + 0x4ede) = 0xe800;
        }
        *(u16*)(self + 0x4686 + n) = *(u16*)(self + 0x4ede);
    }

    if (*(int*)(self + 0x4ec8) >= 0x3800) {
        *(u8*)(self + 0x468b + n) = 1;
    } else {
        *(u8*)(self + 0x468b + n) = 0;
    }

    *(u8*)(self + 0x4ee7) = 1;
    *(u16*)(self + 0x4ee0) = 0;
    func_ov006_020e1dc8(self, idx);

    vol = 0x7f;
    if (*(u8*)(self + 0x468b + n) == 0) vol = 0x3f;
    func_020126ac(0x1d3, 5, vol, 0, func_020126e8(*(int*)(self + 0x4660 + n)));
}
