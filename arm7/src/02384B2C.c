/* storage 0x02384B2C runtime 0x037FC9C4 size 0x5C wram ARM.
 * Pools: =arr_38075C4, =dword_38075A4, =dword_38075A0 (abs32). */

extern unsigned char arr_38075C4[];
extern int dword_38075A4;
extern int dword_38075A0;

int *fn_02384B2C(void) {
    int v0 = 0;
    unsigned char *v1 = arr_38075C4;
    do {
        unsigned char *v2 = &v1[84 * v0];
        *v2 = v0;
        v2[4] = 0;
        v2[3] &= ~1u;
        ++v0;
    } while (v0 < 16);
    dword_38075A4 = 0;
    {
        int *result = &dword_38075A0;
        *result = 0;
        return result;
    }
}
