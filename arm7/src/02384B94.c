/* storage 0x02384B94 runtime 0x037FCA2C size 0x40 wram ARM.
 * Pool: =arr_3807D44 (abs32). Bit0 bitfield: lsl/lsr test + bic/orr RMW. */

extern unsigned char arr_3807D44[];

struct Rec { unsigned char f : 1; };

int fn_02384B94(void) {
    int result;
    for (result = 0; result < 32; ++result) {
        struct Rec *p = (struct Rec *)&arr_3807D44[64 * result];
        if (p->f == 0) {
            p->f = 1;
            return result;
        }
    }
    return -1;
}
