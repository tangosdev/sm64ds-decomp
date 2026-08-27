extern void *func_ov007_020c92ac(void);
extern void func_02046d50(void *a, int b);

void func_ov007_020bc5fc(char *o, int *arr, int n)
{
    int i;

    *(int *)(o + 0x28) = n;
    *(int **)(o + 0x2c) = arr;
    *(void **)(o + 0x30) = func_ov007_020c92ac();
    for (i = 0; i < n; i++) {
        if ((*(int **)(o + 0x2c))[i])
            func_02046d50(**(void ***)o, (*(int **)(o + 0x2c))[i]);
    }
}
