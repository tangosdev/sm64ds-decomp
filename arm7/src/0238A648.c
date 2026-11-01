/* storage 0x0238A648 runtime 0x038024E0 linked 0x134 wram ARM.
 * Setup routine: raises the word_380951C flag, clears unk_3809520's
 * +0x19C word around fn_02389EAC, registers fn_0238A50C, copies a table
 * entry, binds two channels (fn_02381110 pairs on unk_3809544/unk_38096C4
 * with handlers fn_0238A02C/fn_02389F30 and arg a1 / a1-1), then installs
 * the periodic alarm unk_3809840 with handler fn_02389F1C, period 0x1388,
 * start = fn_02381EB0() (u64). */

extern unsigned short word_380951C;
extern int unk_3809520[];
extern int unk_3809534;
extern int unk_3809544;
extern unsigned short unk_3809720[];
extern int unk_38096C4;
extern int unk_3809840;
extern int dword_38096BC;

extern void fn_02389EAC(void);
extern void fn_02382EF8(int *);
extern void fn_023830C8(int, void *);
extern void fn_02381698(int *, int *, int);
extern void fn_02381110(int *, void *, int, int *, int, int);
extern void fn_02380FAC(int *);
extern int fn_0238245C(void);
extern void fn_0238246C(void);
extern void fn_0238244C(int *);
extern unsigned long long fn_02381EB0(void);
extern int fn_0238222C(int *, unsigned long long, unsigned long long, void *, void *);
extern void fn_0238A50C(void);
extern void fn_0238A02C(void);
extern void fn_02389F30(void);
extern void fn_02389F1C(void);

int fn_0238A648(int a1) {
    word_380951C = 1;
    unk_3809520[103] = 1;
    fn_02389EAC();
    unk_3809520[103] = 0;
    fn_02382EF8(unk_3809520);
    fn_023830C8(5, (void *)fn_0238A50C);
    fn_02381698(unk_3809520, &unk_3809534, 4);
    fn_02381110(&unk_3809544, (void *)fn_0238A02C, 0, &dword_38096BC, 0x100, a1);
    fn_02380FAC(&unk_3809544);
    unk_3809720[14] = 0;
    fn_02381110(&unk_38096C4, (void *)fn_02389F30, 0, &unk_3809840, 0x100, a1 - 1);
    fn_02380FAC(&unk_38096C4);
    if (fn_0238245C() == 0)
        fn_0238246C();
    fn_0238244C(&unk_3809840);
    return fn_0238222C(&unk_3809840, fn_02381EB0(), 0x1388, (void *)fn_02389F1C, 0);
}
