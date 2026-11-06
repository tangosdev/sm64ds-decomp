extern unsigned char data_020a1052[];
/* Set link settings byte `byte` of this console, at 0x12 of its own input
   record data_020a1040 (data_020a1052): the writer the 0202 accessors pair
   with GetPlayerFlagByte. */
void func_0203db3c(int byte, int value) {
    data_020a1052[byte] = value;
}
