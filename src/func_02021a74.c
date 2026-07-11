extern int func_02021d1c(char *self, int a1, int id, void *pos, void *p5, void *p6);
extern void func_02021a48(char *table, char *n);

int func_02021a74(char *c, int p1, int p2, void *p3, void *p4)
{
    int i;
    for (i = 0; i < 0x40; i++)
    {
        int idx = *(unsigned char *)(c + 4);
        char *entry = c + idx * 0x1c;
        if (*(int *)(entry + 8) == 0)
        {
            int id;
            char *tableEntry;

            *(int *)c = *(int *)c + 1;
            if (*(int *)c == 0)
            {
                *(int *)c = *(int *)c + 1;
            }

            idx = *(unsigned char *)(c + 4);
            tableEntry = (c + 8) + idx * 0x1c;
            id = *(int *)c;

            if (func_02021d1c(tableEntry, id, p1, (void *)p2, p3, p4) == 0)
            {
                return 0;
            }

            idx = *(unsigned char *)(c + 4);
            tableEntry = (c + 8) + idx * 0x1c;
            func_02021a48(c, tableEntry);
            return *(int *)c;
        }
        else
        {
            idx = idx + 1;
            idx = idx % 0x40;
            *(unsigned char *)(c + 4) = idx;
        }
    }
    return 0;
}
