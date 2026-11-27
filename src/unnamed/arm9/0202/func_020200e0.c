extern int func_0203da9c(void);
extern unsigned char GetLinkByte(int slot, int byte);
extern void SetLinkByte(int byte, int value);
/* daPly_c 0 only: clears the go flag, bit 5 of link settings byte 2
   (set by func_02020124, tested by func_02020168). */
void func_020200e0(void) {
    int x;
    if (func_0203da9c() != 0)
        return;
    x = GetLinkByte(0, 2);
    SetLinkByte(2, (unsigned char)(x & ~0x20));
}
