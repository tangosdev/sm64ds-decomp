// NONMATCHING (11/13): mwccarm always schedules the materialized [lr] load of
// c+0x28 before the folded c+0x34 load and colors the fn-pointer into r3, where the
// ROM uses the reverse load order and ip. Same size; pure scheduling/coloring,
// source order does not move it.
int func_0205c410(char *c)
{
    void *obj = *(void **)(c + 8);
    int a2 = *(int *)(c + 0x28);
    int a1 = *(int *)(c + 0x2c);
    *(int *)(((long long)(int)(c + 0x28)) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(c + 0x34);
    return (*(int (**)(void *, int, int))((char *)obj + 0x40))(obj, a1, a2);
}
