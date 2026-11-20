extern int dword_3806FA4;
extern int fn_02381940(int);
extern int fn_023818E8(int);
extern int fn_023818D4(int, int);
extern int fn_023818C0(int, int);

int fn_023819D8(void) {
    int result = dword_3806FA4;
    if (result != 0)
        return result;
    dword_3806FA4 = 1;
    fn_023818D4(1, fn_02381940(1));
    fn_023818C0(1, fn_023818E8(1));
    fn_023818D4(7, fn_02381940(7));
    fn_023818C0(7, fn_023818E8(7));
    fn_023818D4(8, fn_02381940(8));
    return fn_023818C0(8, fn_023818E8(8));
}
