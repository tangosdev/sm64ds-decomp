extern int _Z13func_0203da9cv(void);
extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
extern void _Z13func_0203db3cii(int byte, int value);

/* Clears bit 3 of this console's link settings byte 0: the ready bit
   func_02020334 sets and func_02020364 tests. */
void func_02020304(void) {
    int v = _Z17GetPlayerFlagByteii(_Z13func_0203da9cv(), 0);
    _Z13func_0203db3cii(0, (unsigned char)(v & ~8));
}
