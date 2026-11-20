extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern int fn_023816E0(int, int);
extern void fn_02380FE0(int);
extern int dword_3806E60[];

int fn_0238176C(int a1) {
    int saved = fn_02382AC4();
    int cur = dword_3806E60[2];
    if (*(int *)(a1 + 4) == cur) {
        --*(int *)(a1 + 8);
        if (*((int *)a1 + 2) == 0) {
            fn_023816E0(cur, a1);
            *(int *)(a1 + 4) = 0;
            fn_02380FE0(a1);
        }
    }
    return fn_02382AD8(saved);
}
