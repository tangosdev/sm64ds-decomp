/* The state block pointer is a char*: with an int-typed base mwcc materializes
   the +0x46xx offset from the literal pool instead of splitting it as
   add r0,r0,#0x4000 / ldr r0,[r0,#0x6xx] the way the cartridge does. */
extern char *data_ov004_020beb68;

int func_ov004_020adc1c(void) {
    char *val = data_ov004_020beb68;
    if (val != 0) {
        return *(int*)(val + 0x4650);
    }
    return 0;
}
