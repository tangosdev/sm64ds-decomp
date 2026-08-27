typedef struct T {
    int *p;
} T;

typedef struct S {
    int n;
    int m;
    T **arr8;
    T **arrc;
} S;

extern void *func_ov007_020c3df4(int a, int size);
extern void func_ov007_020c7348(S *s);
extern T *func_ov007_020c798c(int a0, int a1, int a3, int flag);

S *func_ov007_020c73f0(int a0, int a1, int a2, int a3) {
    int i;
    int j;
    S *s;
    s = (S *)func_ov007_020c3df4(0, 0x10);
    func_ov007_020c7348(s);
    s->arr8 = (T **)func_ov007_020c3df4(0, a2 * 4);
    s->arrc = (T **)func_ov007_020c3df4(0, a1 * 4);
    s->n = a1;
    s->m = a2;
    for (j = 0; j < a2; j++) {
        s->arr8[j] = func_ov007_020c798c(a0, a1, a3, 1);
    }
    for (i = 0; i < a1; i++) {
        s->arrc[i] = func_ov007_020c798c(a0, a2, a3, 0);
        for (j = 0; j < a2; j++) {
            s->arrc[i]->p[j] = s->arr8[j]->p[i];
        }
    }
    return s;
}
