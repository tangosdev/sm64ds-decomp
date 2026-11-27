extern int func_0203da9c(void);
extern unsigned char GetLinkByte(int slot, int byte);
extern void SetLinkByte(int byte, int value);

/* Clears bit 3 of this console's link settings byte 0: the ready bit
   func_02020334 sets and IsPlayerReady tests. */
void func_02020304(void) {
    int v = GetLinkByte(func_0203da9c(), 0);
    SetLinkByte(0, (unsigned char)(v & ~8));
}
