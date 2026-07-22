typedef unsigned char u8;
typedef unsigned short u16;

extern void func_ov006_020e513c(char *self, int idx);
extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);

void func_ov006_020e5e3c(char *self, int idx)
{
    int n, v, w, vol;

    if (*(u8 *)(self + 0x55b8) != 1) return;

    n = idx * 0x30;
    v = (*(int *)(self + 0x5584) - *(int *)(self + 0x4660 + n)) >> 12;
    w = (*(int *)(self + 0x5588) - *(int *)(self + n + 0x4664)) >> 12;
    if (v < -0x2e) return;
    if (v > 0x2e) return;
    if (w < -0x14) return;
    if (w > 0x14) return;

    *(u8 *)(self + n + 0x4688) = 1;
    *(u16 *)(self + 0x4686 + n) = *(u16 *)(self + 0x55b2);
    *(int *)(self + n + 0x4668) = *(int *)(self + 0x559c);

    if (*(u16 *)(self + 0x55b2) < 0x9800u ||
        *(u16 *)(self + 0x55b2) > 0xe800u) {
        if (*(u16 *)(self + 0x55b2) >= 0x4000u &&
            *(u16 *)(self + 0x55b2) <= 0x9800u) {
            *(u16 *)(self + 0x55b2) = 0x9800;
        } else {
            *(u16 *)(self + 0x55b2) = 0xe800;
        }
        *(u16 *)(self + 0x4686 + n) = *(u16 *)(self + 0x55b2);
    }

    if (*(int *)(self + 0x559c) >= 0x3800) {
        *(u8 *)(self + 0x468b + n) = 1;
    } else {
        *(u8 *)(self + 0x468b + n) = 0;
    }

    *(u8 *)(self + 0x55bb) = 1;
    *(u16 *)(self + 0x55b4) = 0;
    *(u8 *)(self + 0x55bf) = 0;
    *(u8 *)(self + 0x55c0) = 0;
    *(u8 *)(self + 0x55c1) = 0;
    func_ov006_020e513c(self, idx);

    vol = 0x7f;
    if (*(u8 *)(self + 0x468b + n) == 0) vol = 0x3f;
    func_020126ac(0x1d3, 5, vol, 0,
                  func_020126e8(*(int *)(self + 0x4660 + n)));
}
