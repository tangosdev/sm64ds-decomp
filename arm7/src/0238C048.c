/* storage 0x0238C048 runtime 0x03803EE0 size 0x38 wram.
 * Pool +0x38: 038098B8, 05000001. */

extern void fn_0238C088(int, int, int);
extern int fn_02380A4C(void);
extern unsigned short unk_38098B8[];

int fn_0238C048(void) {
    int slot;
    int result;

    slot = 0;
    fn_0238C088((int)&slot, (int)unk_38098B8, 0x05000001);
    result = fn_02380A4C();
    unk_38098B8[1] = (unsigned short)result;
    return result;
}
