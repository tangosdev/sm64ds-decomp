/* fn_023885A0: one-shot init of the mailbox driver. */

extern unsigned short word_3808FC0[];
extern int dword_3808FC4[];
extern short unk_38093C4[];
extern int unk_3809244[];
extern int unk_3809258[];
extern int unk_3809298[];
extern int unk_3808FCC[];

extern int fn_02388B80(void);
extern int fn_0238C8A0(int);
extern int fn_0238B8C0(int);
extern int fn_023897B0(int);
extern void fn_02382EF8(int);
extern void fn_023830C8(int, void *);
extern int fn_02381698(int *, int *, int);
extern int fn_02382D2C(void *, int, unsigned int);
extern int fn_02381110(int *, void *, int, int *, int, int);
extern int fn_02380FAC(int *);
extern int fn_023881AC(int, int, int);
extern int fn_0238821C(void);

int fn_023885A0(int a1)
{
    int result = (int)word_3808FC0;
    int v8;
    char *v9;

    if (*(unsigned short *)result == 0) {
        *(unsigned short *)result = 1;
        dword_3808FC4[0] = 0;
        dword_3808FC4[1] = 5;
        result = fn_02388B80();
        result = fn_0238C8A0(result);
        result = fn_0238B8C0(result);
        result = fn_023897B0(result);
        fn_02382EF8(result);
        fn_023830C8(6, (void *)fn_023881AC);
        fn_023830C8(9, (void *)fn_023881AC);
        fn_023830C8(8, (void *)fn_023881AC);
        fn_023830C8(4, (void *)fn_023881AC);
        fn_02381698(unk_3809244, unk_3809258, 16);
        v8 = 0;
        v9 = (char *)unk_3809298;
        do {
            fn_02382D2C(&v9[24 * v8], 0, 24);
            ++v8;
        } while (v8 < 16);
        dword_3808FC4[277] = 0;
        unk_38093C4[44] = 0;
        result = fn_02381110(unk_3808FCC, (void *)fn_0238821C, 0,
                             unk_3809244, 0x200, a1);
        return fn_02380FAC(unk_3808FCC);
    }
    return result;
}
