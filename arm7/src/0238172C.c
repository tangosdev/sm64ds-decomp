extern int fn_023816B8(int);
extern void fn_02380FE0(int);

typedef struct {
    char pad[96];
    int head;
} List172C;

void fn_0238172C(int a1) {
    List172C *list = (List172C *)a1;
    int zero = 0;
    while (list->head != 0) {
        int node = fn_023816B8(a1);
        *(int *)(node + 8) = zero;
        *(int *)(node + 4) = zero;
        fn_02380FE0(node);
    }
}
