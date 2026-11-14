extern int _Z13func_0203da9cv(void);
extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
extern void _Z13func_0203db3cii(int byte, int value);
/* daPly_c 0 only: clears the go flag, bit 5 of link settings byte 2
   (set by func_02020124, tested by func_02020168). */
void func_020200e0(void) {
    int x;
    if (_Z13func_0203da9cv() != 0)
        return;
    x = _Z17GetPlayerFlagByteii(0, 2);
    _Z13func_0203db3cii(2, (unsigned char)(x & ~0x20));
}
