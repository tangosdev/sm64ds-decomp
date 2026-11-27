extern int func_0203da9c(void);
extern unsigned char GetLinkByte(int slot, int byte);
extern void SetLinkByte(int byte, int value);

/* Clears bit 4 of this console's link settings byte 2: the rematch
   ready bit func_020201c0 sets and IsPlayerReadyForRematch tests. */
void func_02020190(void) {
    int v = GetLinkByte(func_0203da9c(), 2);
    SetLinkByte(2, (unsigned char)(v & ~0x10));
}
