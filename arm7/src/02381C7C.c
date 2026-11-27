/* storage 0x02381C7C runtime 0x037F9B14 size 0x114 wram ARM.
 * Aligned malloc: walk the free list for a block covering v11 bytes, split
 * it when the tail is big enough, relink through the in-pack helpers.
 * One data pool word. */

extern int *unk_3806FA8[];
extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern int *fn_02381D94(int *r, int *p);
extern int fn_02381E50(int a1, int *p);
extern int *fn_02381E78(int *a1, int *a2);

int *fn_02381C7C(int a1, int a2, int a3) {
    int v10;
    int *i;
    int v6;
    int *v8;
    int *head;
    v6 = fn_02382AC4();
    v8 = unk_3806FA8[a1];
    if (v8 == 0) {
        fn_02382AD8(v6);
        return 0;
    }
    if (a2 < 0)
        a2 = *v8;
    v10 = v8[4] + 12 * a2;
    a3 = ((int)&((char *)a3)[32] + 31) & ~0x1f;
    head = *(int **)(v10 + 4);
    i = head;
    while (i != 0) {
        if (a3 <= i[2])
            break;
        i = (int *)i[1];
    }
    if (i == 0) {
        fn_02382AD8(v6);
        return 0;
    }
    {
        unsigned int v13 = i[2] - a3;
        if (v13 < 0x40) {
            *(int *)(v10 + 4) = (int)fn_02381E50((int)head, i);
        } else {
            int *v14;
            int *v15;
            i[2] = a3;
            v14 = (int *)((char *)i + a3);
            v14[2] = v13;
            *(int *)((char *)i + a3) = i[0];
            v14[1] = i[1];
            v15 = *(int **)((char *)i + a3 + 4);
            if (v15 != 0)
                *v15 = (int)v14;
            if (*v14 != 0)
                *(int *)(*v14 + 4) = (int)v14;
            else
                *(int *)(v10 + 4) = (int)v14;
        }
        *(int *)(v10 + 8) = (int)fn_02381E78(*(int **)(v10 + 8), i);
        fn_02382AD8(v6);
        return i + 8;
    }
}
