/* func_ov006_02100554 at 0x02100554
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
typedef unsigned char u8;

extern int func_ov004_020adbe0(void);
extern void func_ov006_021004f4(void *c, int a);
extern void func_ov006_021006f4(void *c);

void func_ov006_02100554(void *self) {
    char *s = (char *)self;
    int count;
    int i;

    if (*(u8 *)(s + 0x5299) && *(u8 *)(s + 0x5294)) return;
    if (*(u8 *)(s + 0x52d9) && *(u8 *)(s + 0x52d4)) return;
    if (*(u8 *)(s + 0x5319) && *(u8 *)(s + 0x5314)) return;
    if (*(u8 *)(s + 0x5296) != 0xd && *(u8 *)(s + 0x5294)) return;
    if (*(u8 *)(s + 0x52d6) != 0xd && *(u8 *)(s + 0x52d4)) return;
    if (*(u8 *)(s + 0x5316) != 0xd && *(u8 *)(s + 0x5314)) return;

    count = 0;
    for (i = 0; i < 0x30; i++) {
        if (((u8 (*)[0x40])(s + 0x4698))[i][0]) {
            count++;
            break;
        }
    }
    if (count) return;

    if (func_ov004_020adbe0())
        func_ov006_021004f4(self, 1);
    else
        func_ov006_021004f4(self, 0);
    func_ov006_021006f4(self);
}
