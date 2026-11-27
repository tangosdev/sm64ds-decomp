extern unsigned dword_3805E5C;

unsigned fn_02383828(void) {
    unsigned value = dword_3805E5C * 0x19660D + 0x3C6EF35F;
    dword_3805E5C = value;
    return (unsigned short)(value >> 16);
}
