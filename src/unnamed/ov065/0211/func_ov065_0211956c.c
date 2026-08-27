typedef enum { FALSE = 0, TRUE = 1 } BOOL;
void func_ov065_0211956c(unsigned char *a, void *b) {
    unsigned short v = *(unsigned short*)((char*)b + 0xc);
    BOOL eq = (v == 0xbf) ? TRUE : FALSE;
    if (eq) {
        a += 0x1000;
        *(a + 0x1b5) = 2;
        *(void**)(a + 0x18c) = b;
    }
}
