/* storage 0x023871C4 runtime 0x037FF05C size 0x48 wram.
 * Pool +0x48: 0380877C, 03808780, 03808784, 0380878C. */

extern int dword_380877C;
extern int fn_02380A4C(void);
extern int dword_3808780;
extern int dword_3808784[];
extern int dword_380878C[];
extern int *fn_02397CC8(void);

int *fn_023871C4(int value) {
    dword_380877C = value;
    dword_3808780 = fn_02380A4C();
    dword_3808784[0] = 0;
    dword_3808784[1] = 0;
    dword_380878C[0] = 0;
    dword_380878C[1] = 0;
    return fn_02397CC8();
}
