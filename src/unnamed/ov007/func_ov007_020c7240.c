/* The 0x10-byte grid func_ov007_020c73f0 builds; names as in
 * func_ov007_020ca308. The lists stay void *: this file only hands them to
 * the spring step. */
typedef struct Ov007Grid {
    int columns;        /* 0x00 */
    int rows;           /* 0x04 */
    void **rowLists;    /* 0x08: one per row, columns points each */
    void **columnLists; /* 0x0c: one per column, rows points each */
} Ov007Grid;

typedef char Ov007Grid_size_must_be_0x10[sizeof(Ov007Grid) == 0x10 ? 1 : -1];

extern void func_ov007_020c7560(void* p, int b, int c);

/* Runs func_ov007_020c7560's spring step with (b, d) along every row, then
 * every column. c keeps its char * spelling; `grid` is the typed view. */
void func_ov007_020c7240(char* c, int b, int d) {
    Ov007Grid *grid = (Ov007Grid *)c;
    int i;
    for (i = 0; i < grid->rows; i++) {
        func_ov007_020c7560(grid->rowLists[i], b, d);
    }
    for (i = 0; i < grid->columns; i++) {
        func_ov007_020c7560(grid->columnLists[i], b, d);
    }
}
