extern int _Z13func_0203da9cv(void);
extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
extern void _Z13func_0203db3cii(int byte, int value);

/* Clears bit 4 of this console's link settings byte 2: the rematch
   ready bit func_020201c0 sets and func_020201f0 tests. */
void func_02020190(void) {
    int v = _Z17GetPlayerFlagByteii(_Z13func_0203da9cv(), 2);
    _Z13func_0203db3cii(2, (unsigned char)(v & ~0x10));
}
